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

const unsigned short     smoothBy = 10;                        //Amount of smoothing variables

sf::RectangleShape progressBarBack, progressBarFront;          //Rectangles of progress bar
sf::Text          progressBarTime;                             //Time display on progress bar

unsigned short            barSize = 10;                        //Size of bars
const unsigned short    barAmount = 62;                        //Amount of bars

HSTREAM                   channel;                             //Initialize channel variable
std::vector<std::string>   tracks;                             //Initialize tracks vector, which will hold track paths
int                      trackNow = 0;                         //Index of track used in tracks vector
bool                    isPlaying = true;                      //Is stream playing?

sf::RenderWindow window(sf::VideoMode(cfg.winWidth, cfg.winHeight), cfg.winTitle);

int barWidth = floor((cfg.winWidth*0.7-barAmount+1)/barAmount)-5;

sf::RectangleShape barRect[barAmount];                  //Array of bar rectangles
sf::Sprite sprite;
sf::Texture texture;
std::string title;
std::string author;
double smallk(float x, double y){
	return (x > y) ? x : y;
}

void refreshAlbum(){
	static const char *IdPicture = "APIC";
	TagLib::MPEG::File f(tracks[trackNow].c_str());
	TagLib::ID3v2::Tag *id3v2tag = f.ID3v2Tag();
	TagLib::ID3v2::FrameList Frame ;
	TagLib::ID3v2::AttachedPictureFrame *PicFrame;
	if (id3v2tag){
		Frame = id3v2tag->frameListMap()[IdPicture];
		void *SrcImage;
		unsigned long Size ;
		if (!Frame.isEmpty()){
			for(TagLib::ID3v2::FrameList::ConstIterator it = Frame.begin(); it != Frame.end(); ++it){
				PicFrame = (TagLib::ID3v2::AttachedPictureFrame*)(*it);
				Size = PicFrame->picture().size() ;
				SrcImage = malloc (Size) ;
				if (SrcImage){
					memcpy ( SrcImage, PicFrame->picture().data(), Size ) ;
					if(!texture.loadFromMemory(SrcImage,Size)){
						std::cout << "problem?";
					} else {
						float ratio;
						sprite.setTexture(texture);
						sprite.setPosition(sf::Vector2f(cfg.winWidth/2, cfg.winHeight/2));
						texture.setSmooth(true);
						if(cfg.winWidth>cfg.winHeight){
							ratio = cfg.winWidth/sprite.getLocalBounds().width;
						} else {
							ratio = cfg.winHeight/sprite.getLocalBounds().height;
						}
						sprite.setOrigin(sf::Vector2f(sprite.getLocalBounds().width/2,sprite.getLocalBounds().height/2));
						sprite.setScale(sf::Vector2f(ratio,ratio));
					}
				}
			}
		}
	}
}

//Plays tracks
void playTrack(){
	//Free channel data
	BASS_StreamFree(channel);

	//Diplay in console "playing now"
	std::cout << "-------------------------\nPlaying now: " << static_cast<boost::filesystem::path>(tracks[trackNow]).filename() << std::endl;
	if (trackNow + 1 >= tracks.size()){
		std::cout << "Next: " << static_cast<boost::filesystem::path>(tracks[0]).filename() << "\n\n\n\n";
	} else {
		std::cout << "Next: " << static_cast<boost::filesystem::path>(tracks[trackNow+1]).filename() << "\n\n\n\n";
	}

	std::string toTitle     = tracks[trackNow];
	std::size_t lastSlash   = toTitle.find_last_of("/\\");
	std::size_t lastDot     = toTitle.find_last_of(".");
	//For programs like OBS - creates text file which consists name of song
	if(cfg.saveTitleToFile){
		std::ofstream file("music.txt");
		file << tracks[trackNow].substr(lastSlash+1,lastDot-lastSlash-1);
		file.close();
	}
	std::size_t lastHyphen  = toTitle.find_last_of("-");
	author = tracks[trackNow].substr(lastSlash+1,lastHyphen-lastSlash-1);
	// Author to uppercase
	for(int i=0; i<author.size(); i++){ author[i] = ((int)(author[i]) >= 97 && (int)author[i] <= 122) ? (author[i]&'_') : author[i]; }
	title = tracks[trackNow].substr(lastHyphen+2,lastDot-lastHyphen-2);

	window.setTitle(tracks[trackNow].substr(lastSlash+1,lastDot-lastSlash-1) + " | FrozeT");

	//Set up channel information
	channel = BASS_StreamCreateFile(FALSE, tracks[trackNow].c_str(), 0, 0, 0);
	/*
		channel = BASS_StreamCreateURL(TRACKURL, 0, 0, NULL, 0); //Stream music from URL
		std::cout << BASS_ErrorGetCode();
	*/
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, 0.1f);
	BASS_ChannelPlay(channel, FALSE);
	isPlaying = true;
	refreshAlbum();
}

void shuffle(std::vector<std::string> &before){
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(0, before.size());
	for (int i = 0; i < before.size(); i++){
		std::swap(before[i], before[uni(rng)]);
	}
}

void playNext(std::vector<std::string> args = {}){
	trackNow++;
	if (trackNow >= tracks.size()){
		trackNow = 0;
	}
	playTrack();
}

void playPrevious(std::vector<std::string> args = {}){
	trackNow--;
	if (trackNow < 0){
		trackNow = tracks.size()-1;
	}
	playTrack();
}

void pauseSong(std::vector<std::string> args = {}){
	if(isPlaying){
		BASS_ChannelPause(channel);
		isPlaying = !isPlaying;
	} else {
		BASS_ChannelPlay(channel, FALSE);
		isPlaying = !isPlaying;
	}
}

void windowResizing(unsigned int winW, unsigned int winH){
	cfg.winWidth = winW;
	cfg.winHeight = winH;
	progressBarBack.setSize(sf::Vector2f(cfg.winWidth - 20, 6));
	progressBarBack.setPosition(10, cfg.winHeight - 16);
	progressBarFront.setSize(sf::Vector2f(cfg.winWidth - 20, 6));
	progressBarFront.setPosition(10, cfg.winHeight - 16);
	for (int i = 0; i<barAmount; i++){
		barRect[i].setSize(sf::Vector2f(1, 10));
		barRect[i].setFillColor(sf::Color::White);
		barRect[i].setPosition(cfg.winWidth*0.15 + i * floor((cfg.winWidth*0.7-barAmount+1)/barAmount)+5, cfg.winHeight*0.65-20);
	}
	barWidth = floor((cfg.winWidth*0.7-barAmount+1)/barAmount)-5;
}

std::vector<std::string> takeMusic(boost::filesystem::path p = "F:\\Music\\"){
	std::vector<std::string> files;
	for(auto x : boost::filesystem::directory_iterator(p)){
		if (boost::filesystem::path(x).extension() == ".mp3"){
			boost::filesystem::path fileTemp = x.path();
			files.push_back(fileTemp.string());
		}
	}
	return files;
}

//Button List stored in std::vector
std::vector<Button> buttonList = {
	Button("Next song",10,30,5,5,playNext,{},cfg.lighter_grey),
	Button("Previous song",10,90,5,5,playPrevious,{},cfg.lighter_grey),
	Button("Pause song",10,60,5,5,pauseSong,{},cfg.lighter_grey)
};


/* #######################################################
	 __  __      _         __              _   _          
	|  \/  |__ _(_)_ _    / _|_  _ _ _  __| |_(_)___ _ _  
	| |\/| / _` | | ' \  |  _| || | ' \/ _|  _| / _ \ ' \ 
	|_|  |_\__,_|_|_||_| |_|  \_,_|_||_\__|\__|_\___/_||_|  

   #######################################################   */

int main(){
	tracks = takeMusic();
	shuffle(tracks);
	float fft[2048];                                        //FFT array (1/2 of freq of samples)
	float bars[barAmount];                                  //Array holding bars values
	float smoothingBars[barAmount][smoothBy] = { 0 };       //Array holding variables used for smoothing bars
	double time, duration;                                  //Time and duration of track
	sf::FloatRect bounds;                                   //Bounds of time of progress bar
	const sf::Color winBackground = cfg.grey;                  //Color of the window's background

	sf::RectangleShape albumCover;                                  //Album cover image near title and author
	sf::Text titleText;
	sf::Text authorText;


	//Initialize BASS and play track
	BASS_Init(cfg.deviceID, cfg.freq, 0, 0, NULL);
	playTrack();

	//Initialize window frame
	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);

	//Change text style and font
	progressBarTime.setFont(cfg.fBold);
	progressBarTime.setCharacterSize(18);
	progressBarTime.setFillColor(sf::Color::White);

	//Set progress bar
	progressBarBack.setSize(sf::Vector2f(cfg.winWidth - 20, 6));
	progressBarBack.setPosition(10, cfg.winHeight - 16);
	progressBarBack.setFillColor(sf::Color(40, 40, 40, 255));
	progressBarFront.setSize(sf::Vector2f(cfg.winWidth - 20, 6));
	progressBarFront.setPosition(10, cfg.winHeight - 16);
	progressBarFront.setFillColor(sf::Color::White);

	for (int i = 0; i<barAmount; i++){
		barRect[i].setSize(sf::Vector2f(1, 10));
		barRect[i].setFillColor(sf::Color::White);
		barRect[i].setPosition(cfg.winWidth*0.15 + i * floor((cfg.winWidth*0.7-barAmount+1)/barAmount)+5, cfg.winHeight*0.65-20);
	}

	while (window.isOpen()){
		//Set time and duration of track
		time = BASS_ChannelBytes2Seconds(channel, BASS_ChannelGetPosition(channel, BASS_POS_BYTE));
		duration = BASS_ChannelBytes2Seconds(channel, BASS_ChannelGetLength(channel, BASS_POS_BYTE));
		progressBarFront.setSize(sf::Vector2f((cfg.winWidth - 20)*BASS_ChannelGetPosition(channel, BASS_POS_BYTE) / BASS_ChannelGetLength(channel, BASS_POS_BYTE), 6));
		if(BASS_ChannelIsActive(channel) == BASS_ACTIVE_PLAYING){
			// Music is playing STATE
		} else if(BASS_ChannelIsActive(channel) == BASS_ACTIVE_PAUSED){
			// Music is paused STATE
		} else {
			playNext();
		}

		//Save GPU power while unfocused
		//if(!window.hasFocus() && cfg.saveEnergy == true){
		//	window.setFramerateLimit(15);
		//} else {
		//	window.setFramerateLimit(cfg.winFPS);
		//}

		//Change progress bar time
		progressBarTime.setString(toHumanTime(time) + "/" + toHumanTime(duration));
		bounds = progressBarTime.getLocalBounds();
		progressBarTime.setPosition(cfg.winWidth - 10 - bounds.width, cfg.winHeight - 42);

		//Set bars values
		BASS_ChannelGetData(channel, fft, cfg.fftfreq);
		for (int a = 0; a<barAmount; a++){
			float sum = 0;
			for (int j = 0; j<smoothBy - 1; j++){
				smoothingBars[a][j] = smoothingBars[a][j + 1];
				sum += smoothingBars[a][j];
			}
			smoothingBars[a][smoothBy - 1] = fft[a];
			sum += smoothingBars[a][smoothBy - 1];
			bars[a] = sqrt(sum / smoothBy)*-1*(cfg.winHeight*0.6) - 1;
		}

		//Take average of bars values
		float aveBars[barAmount];
		for (int i = 0; i<barAmount; i++){
			aveBars[i] = (bars[std::max(0, i - 1)] + bars[i] + bars[std::min(barAmount - 1, i + 1)]) / 3;
		}

		//Handle events
		sf::Event event;
		while (window.pollEvent(event)){
			switch (event.type){
				//If window is coming to close
				case sf::Event::Closed:
					window.close();
					return 1;
					break;
				//If window is resized
				case sf::Event::Resized:
					window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
					windowResizing(event.size.width, event.size.height);
					refreshAlbum();
					break;
				//If key is pressed
				case sf::Event::KeyPressed:
					if(event.key.code == sf::Keyboard::P){
						playNext();
					}
					break;
				//If button is pressed
				case sf::Event::MouseButtonPressed:
					if(event.mouseButton.button == 0){
						for(int i=0; i<buttonList.size(); i++){
							buttonList[i].checkIfClicked(sf::Vector2i(event.mouseButton.x,event.mouseButton.y));
						}
					}
					break;
				default:
					break;
			}
		}

		albumCover.setTexture(&texture);
		albumCover.setPosition(cfg.winWidth*0.15,cfg.winHeight*0.65);
		albumCover.setSize(sf::Vector2f(128,128));
		authorText.setString(author);
		authorText.setFont(cfg.fBold);
		authorText.setCharacterSize(50);
		authorText.setPosition(cfg.winWidth*0.15+150,cfg.winHeight*0.65+10);
		titleText.setString(title);
		titleText.setFont(cfg.fRegular);
		titleText.setCharacterSize(30);
		titleText.setPosition(cfg.winWidth*0.15+150,cfg.winHeight*0.65+70);

		//Draw everything on window
		window.clear(winBackground);
		window.draw(sprite,&cfg.shader_brightness);
		for (int i = 0; i<barAmount; i++){
			barRect[i].setSize(sf::Vector2f(barWidth, smallk(aveBars[i],cfg.winHeight*-0.35) ));
			window.draw(barRect[i]);
		}
		window.draw(progressBarBack);
		window.draw(progressBarFront);
		window.draw(progressBarTime);
		for(int i=0; i<buttonList.size(); i++){
			buttonList[i].draw(window);
		}
		window.draw(albumCover);
		window.draw(authorText);
		window.draw(titleText);
		window.display();
	}

	return 0;
}