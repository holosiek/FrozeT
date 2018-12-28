//SFML
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
//Boost
#include <boost/filesystem.hpp>
//Standard
#include <iostream>
#include <string>
#include <math.h>
#include <cmath>
#include <vector>
#include <random>
#include <fstream>
#include <shlobj.h>
#include <windows.h>
//TagLib
#include <tag.h>
#include <fileref.h>
#include <mpegfile.h>
#include <id3v1tag.h>
#include <id3v2tag.h>
#include <id3v2frame.h>
#include <id3v2header.h>
#include <attachedpictureframe.h>
//Bass
#include "bass.h"
//Project Related
#include "mymisc.h"
#include "config.h"

const unsigned short     smoothBy = 9;                        //Amount of smoothing variables

unsigned short            barSize = 10;                        //Size of bars
const unsigned short    barAmount = 62;                        //Amount of bars

HSTREAM                   channel = 0;                         //Initialize channel variable
std::vector<std::wstring>  tracks;                             //Initialize tracks vector, which will hold track paths
int                      trackNow = 0;                         //Index of track used in tracks vector
bool                    isPlaying = true;                      //Is stream playing?

sf::RenderWindow window(sf::VideoMode(cfg.winWidth, cfg.winHeight), cfg.winTitle);

int barWidth = floor((cfg.winWidth*0.7-barAmount+1)/barAmount)-5;


std::wstring title, author;                                    // Variables holding title and author of music
sf::RectangleShape progressBarBack, progressBarFront;          // Rectangles of progress bar
sf::Text progressBarTime;                                      // Time display on progress bar
sf::RectangleShape barRect[barAmount];                         // Visualizer bars
sf::RectangleShape barRectShadow[barAmount];                   // Visualizer bars shadow
sf::Sprite albumCoverSprite;                                   // Sprite of album cover image
sf::Texture texture;                                           // Texture of album cover image
sf::RectangleShape albumCover;                                 // Album cover image near title and author
sf::Text titleText; sf::Text titleTextShadow;                  // Title text and it's shadow
sf::Text authorText; sf::Text authorTextShadow;                // Author text and it's shadow

/*
################################## File System Dialog functions
*/

// Handles our file system dialog
static int CALLBACK dialogHandler(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData){
	// [DEBUG] Check whenever selection in dialog changed
	if(uMsg == BFFM_SELCHANGED && cfg.debugMode == true){
		TCHAR path[MAX_PATH];
		SHGetPathFromIDList((LPITEMIDLIST) lParam, path);
		std::cout << cfg.debugPrefix << path << std::endl;
	}
    return 0;
}

static std::string utf16ToUTF8( const std::wstring &s )
{
    const int size = ::WideCharToMultiByte( CP_UTF8, 0, s.c_str(), -1, NULL, 0, 0, NULL );

    std::vector<char> buf( size );
    ::WideCharToMultiByte( CP_UTF8, 0, s.c_str(), -1, &buf[0], size, 0, NULL );

    return std::string( &buf[0] );
}

// Show file system dialog
std::string browseFilesDialog(std::wstring const dialogTitle = L""){
    TCHAR path[MAX_PATH];
    BROWSEINFO dialogOptions            = { 0 };
			   dialogOptions.lpszTitle  = utf16ToUTF8(dialogTitle).c_str();
			   dialogOptions.ulFlags    = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
		       dialogOptions.lpfn       = dialogHandler;
		       dialogOptions.lParam     = NULL;
    LPITEMIDLIST pidl = SHBrowseForFolder(&dialogOptions);

	// If file system dialog is not canceled
    if(pidl != 0){
        // Convert our SHBrowseForFolder result to normal path name and save it to "path"
        SHGetPathFromIDList(pidl, path);

        // Free memory from "pidl"
        IMalloc *imalloc = nullptr;
        if (SUCCEEDED(SHGetMalloc(&imalloc))){
            imalloc->Free(pidl);
            imalloc->Release();
        }
        return path;
    }
    return "";
}

/*
################################## Load album image from file using TagLib
*/

void refreshAlbum(){
	if(tracks.size() != 0){
		// Load MP3 file which will be used to extract album cover and check for ID3v2 tag
		TagLib::MPEG::File f(tracks[trackNow].c_str());
		TagLib::ID3v2::Tag *id3v2tag = f.ID3v2Tag();

		// If ID3v2 tag exist...
		if(id3v2tag){
			// Get frame list with "APIC" ID and check if frame is not empty
			TagLib::ID3v2::FrameList frame = id3v2tag->frameListMap()["APIC"];
			if(!frame.isEmpty()){
				// Loop for every frame found in list
				for(TagLib::ID3v2::FrameList::ConstIterator i = frame.begin(); i != frame.end(); i++){
					// Extract album cover as ByteVector and allocate memory
					TagLib::ID3v2::AttachedPictureFrame *picFrame = (TagLib::ID3v2::AttachedPictureFrame*)(*i);
					unsigned long picSize = picFrame->picture().size();
					void *outImage = malloc(picSize);
					if (outImage){
						// Copy to allocated memory whole picture data
						memcpy(outImage, picFrame->picture().data(), picSize);

						// Load texture
						if(!texture.loadFromMemory(outImage,picSize)){
							std::cout << "problem?";
						} else {
							// Set albumCoverSprite texture and position it on window
							float ratio;
							albumCoverSprite.setTexture(texture);
							albumCoverSprite.setPosition(sf::Vector2f(cfg.winWidth/2, cfg.winHeight/2));
							texture.setSmooth(true);
							if(cfg.winWidth>cfg.winHeight){
								ratio = cfg.winWidth/albumCoverSprite.getLocalBounds().width;
							} else {
								ratio = cfg.winHeight/albumCoverSprite.getLocalBounds().height;
							}
							albumCoverSprite.setOrigin(sf::Vector2f(albumCoverSprite.getLocalBounds().width/2,albumCoverSprite.getLocalBounds().height/2));
							albumCoverSprite.setScale(sf::Vector2f(ratio,ratio));
						}
					}
				}
			}
		}
	}
}

/*
################################## Play track
*/

void playTrack(){
	if(tracks.size() != 0){
		// Free channel data
		if(channel != 0 && !BASS_StreamFree(channel)){
			std::cout << BASS_ErrorGetCode() << std::endl;
			exit(EXIT_FAILURE);
		}

		// Diplay in console "Playing now"
		std::cout << "-------------------------\nPlaying now: " << static_cast<boost::filesystem::path>(tracks[trackNow]).filename() << std::endl;
		if (trackNow + 1 >= tracks.size()){
			std::cout << "Next: " << static_cast<boost::filesystem::path>(tracks[0]).filename() << "\n\n\n\n";
		} else {
			std::cout << "Next: " << static_cast<boost::filesystem::path>(tracks[trackNow+1]).filename() << "\n\n\n\n";
		}

		// Find title and author by pattern "Author - Title" in music filename
		//std::string toTitle( tracks[trackNow].begin(), tracks[trackNow].end() );
		std::wstring toTitle    = tracks[trackNow];
		std::size_t lastSlash   = toTitle.find_last_of(L"/\\");
		std::size_t lastDot     = toTitle.find_last_of(L".");
		std::size_t lastHyphen  = toTitle.find_last_of(L"-");
		author = tracks[trackNow].substr(lastSlash+1,lastHyphen-lastSlash-1);
	
		// Set author name to uppercase and save it to "title" var
		for(int i=0; i<author.size(); i++){ 
			author[i] = ((int)(author[i]) >= 97 && (int)author[i] <= 122) ? (author[i]&'_') : author[i]; 
		}
		title = tracks[trackNow].substr(lastHyphen+2,lastDot-lastHyphen-2);

		// Change window name to music name
		window.setTitle(tracks[trackNow].substr(lastSlash+1,lastDot-lastSlash-1) + L" | FrozeT");

		// For programs like OBS - creates text file which consists name of song
		if(cfg.saveTitleToFile){
			std::ofstream file("music.txt");
			file << tracks[trackNow].substr(lastSlash+1,lastDot-lastSlash-1);
			file.close();
		}

		/*
			Will be used later for streaming from url, just saved this snippet in code
			channel = BASS_StreamCreateURL(TRACKURL, 0, 0, NULL, 0); //Stream music from URL
		*/

		// Set up channel by loading music from file and setting channel variables
		channel = BASS_StreamCreateFile(FALSE, tracks[trackNow].c_str(), 0, 0, 0);
		BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, 0.3f);
		BASS_ChannelPlay(channel, FALSE);
		isPlaying = true;

		// Load album image
		refreshAlbum();
	}
}

/*
################################## Shuffle tracks
*/

void shuffle(std::vector<std::wstring> &before){
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(0, before.size()-1);
	for (int i = 0; i < before.size(); i++){
		std::swap(before[i], before[uni(rng)]);
	}
}

/*
################################## Play next song
*/

void playNext(std::vector<std::wstring> args = {}){
	trackNow++;
	if(trackNow >= tracks.size()){
		trackNow = 0;
	}
	playTrack();
}

/*
################################## Play previous song
*/

void playPrevious(std::vector<std::wstring> args = {}){
	trackNow--;
	if(trackNow < 0){
		trackNow = tracks.size()-1;
	}
	playTrack();
}

/*
################################## Pause/Resume song
*/

void pauseSong(std::vector<std::wstring> args = {}){
	if(isPlaying){
		BASS_ChannelPause(channel);
		isPlaying = !isPlaying;
	} else {
		BASS_ChannelPlay(channel, FALSE);
		isPlaying = !isPlaying;
	}
}

/*
################################## Reposition objects on window resize
*/

void windowResizing(unsigned int winW = cfg.winWidth, unsigned int winH = cfg.winHeight){
	// Set new window's height and width to variable
	cfg.winWidth = winW;
	cfg.winHeight = winH;

	// Reposition/Resize elements
	progressBarBack.setSize(sf::Vector2f(cfg.winWidth - 20, 6));
	progressBarBack.setPosition(10, cfg.winHeight - 16);
	progressBarFront.setSize(sf::Vector2f(cfg.winWidth - 20, 6));
	progressBarFront.setPosition(10, cfg.winHeight - 16);

	authorText.setPosition(cfg.winWidth*0.15+150,cfg.winHeight*0.65+10);
	authorTextShadow.setPosition(cfg.winWidth*0.15+152,cfg.winHeight*0.65+12);
	titleText.setPosition(cfg.winWidth*0.15+150,cfg.winHeight*0.65+70);
	titleTextShadow.setPosition(cfg.winWidth*0.15+152,cfg.winHeight*0.65+72);
	albumCover.setPosition(cfg.winWidth*0.15,cfg.winHeight*0.65);

	for (int i = 0; i<barAmount; i++){
		barRect[i].setSize(sf::Vector2f(1, 10));
		barRect[i].setFillColor(sf::Color::White);
		barRect[i].setPosition(cfg.winWidth*0.15 + i * floor((cfg.winWidth*0.7-barAmount+1)/barAmount)+5, cfg.winHeight*0.65-20);
		barRectShadow[i].setSize(sf::Vector2f(1, 10));
		barRectShadow[i].setFillColor(sf::Color::Black);
		barRectShadow[i].setPosition(cfg.winWidth*0.15 + i * floor((cfg.winWidth*0.7-barAmount+1)/barAmount)+5+2, cfg.winHeight*0.65-20+2);
	}
	barWidth = floor((cfg.winWidth*0.7-barAmount+1)/barAmount)-5;
}

/*
################################## Save some GPU power by decreasing FPS while unfocused
*/

void saveMode(bool state){
	if(state && cfg.saveEnergy){
		window.setFramerateLimit(15);
	} else {
		window.setFramerateLimit(cfg.winFPS);
	}
}

/*
################################## Search folder for music and return list of music paths
*/

std::vector<std::wstring> takeMusic(boost::filesystem::path p){
	std::vector<std::wstring> files;
	for(auto x : boost::filesystem::directory_iterator(p)){
		if (boost::filesystem::path(x).extension() == ".mp3"){
			boost::filesystem::path fileTemp = x.path();
			files.push_back(fileTemp.wstring());
		}
	}
	return files;
}

/*
################################## Open folder, prepare music, shuffle
*/

void openTakeShuffleMusic(std::vector<std::wstring> args = {L""}){
	if(args[0] == L""){
		// Stop, free and clear all variables used for playing
		isPlaying = false;
		BASS_ChannelStop(channel);
		tracks.clear();
		trackNow = 0;
		// Take music from working directory, then shuffle and play it
		tracks = takeMusic(boost::filesystem::current_path());
		shuffle(tracks);
		playTrack();
		// [DEBUG] Print how many songs are in this folder
		if(cfg.debugMode == true){
			std::cout << cfg.debugPrefix << tracks.size() << std::endl;
		}
	} else {
		std::string path = browseFilesDialog(args[0]);
		if(path != ""){
			// Stop, free and clear all variables used for playing
			isPlaying = false;
			BASS_ChannelStop(channel);
			tracks.clear();
			trackNow = 0;
			// Take music from path we chose, then shuffle and play it
			tracks = takeMusic((boost::filesystem::path) path);
			shuffle(tracks);
			playTrack();
			// [DEBUG] Print how many songs are in this folder
			if(cfg.debugMode == true){
				std::cout << cfg.debugPrefix << tracks.size() << std::endl;
			}
		}
	}
}

/*
################################## Button List stored in std::vector
*/

std::vector<Button> buttonList = {
	Button(L"Next song",10,30,5,5,playNext,{},cfg.lighter_grey),
	Button(L"Previous song",10,90,5,5,playPrevious,{},cfg.lighter_grey),
	Button(L"Pause song",10,60,5,5,pauseSong,{},cfg.lighter_grey),
	Button(L"Open Folder",10,120,5,5,openTakeShuffleMusic,{L"Open folder which holds music"},cfg.lighter_grey)
};


/* #######################################################
	 __  __      _         __              _   _          
	|  \/  |__ _(_)_ _    / _|_  _ _ _  __| |_(_)___ _ _  
	| |\/| / _` | | ' \  |  _| || | ' \/ _|  _| / _ \ ' \ 
	|_|  |_\__,_|_|_||_| |_|  \_,_|_||_\__|\__|_\___/_||_|  

   #######################################################   */

int main(){
	float fft[2048];                                        //FFT array (1/2 of freq of samples)
	float bars[barAmount];                                  //Array holding bars values
	float smoothingBars[barAmount][smoothBy] = { 0 };       //Array holding variables used for smoothing bars
	double time, duration;                                  //Time and duration of track
	const sf::Color winBackground = cfg.grey;               //Color of the window's background
	float aveBars[barAmount];

	//######################################################### INITALIZE EVERYTHING

	// Folder containing music
	boost::filesystem::path musicFolder = "";
	std::ifstream configFile("config.txt");
	configFile >> musicFolder;
	configFile >> cfg.deviceID;
	configFile.close();

	// Initialize BASS and play track
	if(!BASS_Init(cfg.deviceID, cfg.freq, 0, NULL, NULL)){
		std::cout << BASS_ErrorGetCode() << std::endl;
		exit(EXIT_FAILURE);
	};
	openTakeShuffleMusic({L""});

	// Initialize window frame settings
	window.setVerticalSyncEnabled(false);
	window.setFramerateLimit(cfg.winFPS);
	window.setKeyRepeatEnabled(false);

	// ######################################################### CHANGE STYLES

	// Change "duration and current time" text style
	progressBarTime.setFont(cfg.fBold);
	progressBarTime.setCharacterSize(18);
	progressBarTime.setFillColor(sf::Color::White);

	// Change "progress bar background" style
	progressBarBack.setSize(sf::Vector2f(cfg.winWidth - 20, 6));
	progressBarBack.setPosition(10, cfg.winHeight - 16);
	progressBarBack.setFillColor(sf::Color(40, 40, 40, 255));

	// Change "progress bar front" style
	progressBarFront.setSize(sf::Vector2f(cfg.winWidth - 20, 6));
	progressBarFront.setPosition(10, cfg.winHeight - 16);
	progressBarFront.setFillColor(sf::Color::White);

	// Change "album cover image" style
	albumCover.setTexture(&texture);
	albumCover.setSize(sf::Vector2f(128,128));
	albumCover.setPosition(cfg.winWidth*0.15,cfg.winHeight*0.65);

	// Change "artist text" and it's shadow style
	authorText.setFont(cfg.fBold);
	authorText.setCharacterSize(50);
	authorText.setPosition(cfg.winWidth*0.15+150,cfg.winHeight*0.65+10);
	authorTextShadow.setFont(cfg.fBold);
	authorTextShadow.setFillColor(sf::Color::Black);
	authorTextShadow.setCharacterSize(50);
	authorTextShadow.setPosition(cfg.winWidth*0.15+152,cfg.winHeight*0.65+12);

	// Change "title text" and it's shadow style
	titleText.setFont(cfg.fRegular);
	titleText.setCharacterSize(30);
	titleText.setPosition(cfg.winWidth*0.15+150,cfg.winHeight*0.65+70);
	titleTextShadow.setFont(cfg.fRegular);
	titleTextShadow.setFillColor(sf::Color::Black);
	titleTextShadow.setCharacterSize(30);
	titleTextShadow.setPosition(cfg.winWidth*0.15+152,cfg.winHeight*0.65+72);

	// Change "visualizer bars" style
	for(int i=0; i<barAmount; i++){
		barRect[i].setSize(sf::Vector2f(1, 10));
		barRect[i].setFillColor(sf::Color::White);
		barRect[i].setPosition(cfg.winWidth*0.15 + i * floor((cfg.winWidth*0.7-barAmount+1)/barAmount)+5, cfg.winHeight*0.65-20);
		barRectShadow[i].setSize(sf::Vector2f(1, 10));
		barRectShadow[i].setFillColor(sf::Color::Black);
		barRectShadow[i].setPosition(cfg.winWidth*0.15 + i * floor((cfg.winWidth*0.7-barAmount+1)/barAmount)+5+1, cfg.winHeight*0.65-20+1);
	}

	cfg.shader_brightness.setUniform("blur_radius", sf::Vector2f(0.003f, 0.003f));
	cfg.shader_glass.setUniform("blur_radius", sf::Vector2f(0.001f, 0.001f));

	//######################################################### WINDOW LOOP

	while (window.isOpen()){
		// Get time and duration of track and set size of prograss bar
		time = BASS_ChannelBytes2Seconds(channel, BASS_ChannelGetPosition(channel, BASS_POS_BYTE));
		duration = BASS_ChannelBytes2Seconds(channel, BASS_ChannelGetLength(channel, BASS_POS_BYTE));
		progressBarFront.setSize(sf::Vector2f((cfg.winWidth - 20)*BASS_ChannelGetPosition(channel, BASS_POS_BYTE) / BASS_ChannelGetLength(channel, BASS_POS_BYTE), 6));
		
		// Check state of channel
		switch(BASS_ChannelIsActive(channel)){
			case BASS_ACTIVE_PLAYING: // PLAYING STATE
				break;
			case BASS_ACTIVE_PAUSED:  // PAUSED STATE
				break;
			default:
				playNext();
				break;
		}

		// Change progress bar text and position
		progressBarTime.setString(toHumanTime(time) + "/" + toHumanTime(duration));
		progressBarTime.setPosition(cfg.winWidth-10-progressBarTime.getLocalBounds().width, cfg.winHeight-42);

		//######################################################### VISUALIZER

		// Take channel data, change it into visualizer bar data and smooth each bar
		BASS_ChannelGetData(channel, fft, cfg.fftfreq);
		for(int i=0; i<barAmount; i++){
			float sum = 0;
			for(int j = 0; j<smoothBy-1; j++){
				smoothingBars[i][j] = smoothingBars[i][j+1];
				sum += smoothingBars[i][j];
			}
			smoothingBars[i][smoothBy-1] = fft[i];
			sum += smoothingBars[i][smoothBy-1];
			// Set value of each bar by formula: -1 (to flip upside down bar) * square root of -> divide sum of smoothing values by smoothing var * 60% - 1px of window's height
			bars[i] = -1.0f*sqrt(sum/smoothBy)*(cfg.winHeight*0.6f)-1.0f;
		}

		// Smooth all bars by taking average of neighbours and set their size
		for(int i=0; i<barAmount; i++){
			aveBars[i] = (bars[std::max(0, i - 1)] + bars[i] + bars[std::min(barAmount - 1, i + 1)]) / 3;
			barRect[i].setSize(sf::Vector2f(barWidth, biggerFloatOrDouble(aveBars[i],cfg.winHeight*-0.35)));
			barRectShadow[i].setSize(sf::Vector2f(barWidth, biggerFloatOrDouble(aveBars[i],cfg.winHeight*-0.35)));
		}

		//######################################################### WINDOW EVENTS

		sf::Event event;
		while(window.pollEvent(event)){
			switch (event.type){
				// On window close
				case sf::Event::Closed:
					window.close();
					return 1;
					break;
				// On window resize
				case sf::Event::Resized:
					window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
					windowResizing(event.size.width, event.size.height);
					refreshAlbum();
					break;
				// On key press
				case sf::Event::KeyPressed:
					if(event.key.code == sf::Keyboard::P){
						playNext();
					}
					break;
				// On mouse press
				case sf::Event::MouseButtonPressed:
					if(event.mouseButton.button == 0){
						for(int i=0; i<buttonList.size(); i++){
							buttonList[i].checkIfClicked(sf::Vector2i(event.mouseButton.x,event.mouseButton.y));
						}
					}
					break;
				// On lost window focus
				case sf::Event::LostFocus:
					saveMode(true);
					break;
				// On gain window focus
				case sf::Event::GainedFocus:
					saveMode(false);
					break;
				default:
					break;
			}
		}

		// Set artist and song title text
		authorText.setString(author);
		authorTextShadow.setString(author);
		titleText.setString(title);
		titleTextShadow.setString(title);

		// Draw everything on the window
		window.clear(winBackground);

		window.draw(albumCoverSprite,&cfg.shader_brightness);
		for(int i=0; i<barAmount; i++){
			window.draw(barRectShadow[i]);
			window.draw(barRect[i]);
		}
		window.draw(albumCover);
		window.draw(authorTextShadow);
		window.draw(authorText);
		window.draw(titleTextShadow);
		window.draw(titleText);
		window.draw(progressBarBack);
		window.draw(progressBarFront);
		window.draw(progressBarTime);
		for(int i=0; i<buttonList.size(); i++){
			buttonList[i].draw(window);
		}
		window.display();
	}

	return 0;
}