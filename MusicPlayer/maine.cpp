//SFML
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
//Boost
#include <boost/filesystem.hpp>
//Standard
#include <iostream>
#include <string>
#include <math.h>
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

const short              deviceID = 4;                         //DeviceID of device that will play music, -1 = default
const unsigned short         freq = 44100;                     //Frequency of channel
const unsigned short     fftarray = 2048;                      //Length of FFT array
const unsigned short     smoothBy = 10;                        //Amount of smoothing variables
const DWORD               fftfreq = BASS_DATA_FFT4096;         //Freq of samples

unsigned short           winWidth = 960;                       //Window width
unsigned short          winHeight = 480;                       //Window height
const unsigned short       winFPS = 60;                        //Window fps
const std::string        winTitle = "Ceplusplus";              //Window title
sf::Color           winBackground = cfg.grey;                  //Color of the window's background

sf::RectangleShape progressBarBack, progressBarFront;          //Rectangles of progress bar
sf::Text          progressBarTime;                             //Time display on progress bar

unsigned short            barSize = 10;                        //Size of bars
const unsigned short    barAmount = 62;                        //Amount of bars

HSTREAM                   channel;                             //Initialize channel variable
std::vector<std::string>   tracks;                             //Initialize tracks vector, which will hold track paths
int                      trackNow = 0;                         //Index of track used in tracks vector
bool                    isPlaying = true;                      //Is stream playing?

sf::Sprite sprite;
sf::Texture texture;
sf::Shader shader;
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
						sprite.setPosition(sf::Vector2f(winWidth/2, winHeight/2));
						texture.setSmooth(true);
						if(winWidth>winHeight){
							ratio = winWidth/sprite.getLocalBounds().width;
						} else {
							ratio = winHeight/sprite.getLocalBounds().height;
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

	//For programs like OBS - creates text file which consists name of song
	if(cfg.saveTitleToFile){
		std::ofstream file("music.txt");
		std::string toTitle   = tracks[trackNow];
		std::size_t lastSlash = toTitle.find_last_of("/\\");
		std::size_t lastDot   = toTitle.find_last_of(".");
		file << tracks[trackNow].substr(lastSlash+1,lastDot-lastSlash-1);
		file.close();
	}

	//Set up channel information
	channel = BASS_StreamCreateFile(FALSE, tracks[trackNow].c_str(), 0, 0, 0);
	/*
		channel = BASS_StreamCreateURL(TRACKURL, 0, 0, NULL, 0); //Stream music from URL
		std::cout << BASS_ErrorGetCode();
	*/
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, 0.1);
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
	winWidth = winW;
	winHeight = winH;
	progressBarBack.setSize(sf::Vector2f(winWidth - 20, 6));
	progressBarBack.setPosition(10, winHeight - 16);
	progressBarFront.setSize(sf::Vector2f(winWidth - 20, 6));
	progressBarFront.setPosition(10, winHeight - 16);
}

std::vector<std::string> takeMusic(boost::filesystem::path p = "F:\\Music\\"){
	std::vector<std::string> files;
	for(auto&& x : boost::filesystem::directory_iterator(p)){
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

int main(){
	if (!shader.loadFromFile("shad.vert", "shad.frag")){
		std::cout << "problem?";
	}

	tracks = takeMusic();
	shuffle(tracks);
	sf::RectangleShape barRect[barAmount];                  //Array of bar rectangles
	float fft[fftarray];                                    //FFT array (1/2 of freq of samples)
	float bars[barAmount];                                  //Array holding bars values
	float smoothingBars[barAmount][smoothBy] = { 0 };       //Array holding variables used for smoothing bars
	double time, duration;                                  //Time and duration of track
	sf::Text text;                                          //Text variable displaying time and duration
	sf::FloatRect bounds;                                   //Bounds of time of progress bar

	//Initialize BASS and play track
	BASS_Init(deviceID, freq, 0, 0, NULL);
	playTrack();

	//Initialize window frame
	sf::RenderWindow window(sf::VideoMode(winWidth, winHeight), winTitle);
	window.setVerticalSyncEnabled(false);
	window.setKeyRepeatEnabled(false);

	//Change text style and font
	text.setFont(cfg.fBold);
	text.setCharacterSize(24);
	text.setFillColor(sf::Color::White);
	progressBarTime.setFont(cfg.fBold);
	progressBarTime.setCharacterSize(18);
	progressBarTime.setFillColor(sf::Color::White);

	//Set progress bar
	progressBarBack.setSize(sf::Vector2f(winWidth - 20, 6));
	progressBarBack.setPosition(10, winHeight - 16);
	progressBarBack.setFillColor(sf::Color(40, 40, 40, 255));
	progressBarFront.setSize(sf::Vector2f(winWidth - 20, 6));
	progressBarFront.setPosition(10, winHeight - 16);
	progressBarFront.setFillColor(sf::Color::White);

	for (int i = 0; i<barAmount; i++){
		barRect[i].setSize(sf::Vector2f(1, 10));
		barRect[i].setFillColor(sf::Color::White);
		barRect[i].setPosition(10 + i * 11, winHeight - 10);
	}

	while (window.isOpen()){
		//Set time and duration of track
		time = BASS_ChannelBytes2Seconds(channel, BASS_ChannelGetPosition(channel, BASS_POS_BYTE));
		duration = BASS_ChannelBytes2Seconds(channel, BASS_ChannelGetLength(channel, BASS_POS_BYTE));
		progressBarFront.setSize(sf::Vector2f((winWidth - 20)*BASS_ChannelGetPosition(channel, BASS_POS_BYTE) / BASS_ChannelGetLength(channel, BASS_POS_BYTE), 6));
		if(BASS_ChannelIsActive(channel) == BASS_ACTIVE_PLAYING){
			text.setString(tracks[trackNow]);
		} else if(BASS_ChannelIsActive(channel) == BASS_ACTIVE_PAUSED){
			text.setString("paused");
		} else {
			playNext();
			text.setString("end");
		}

		//Save GPU power while unfocused
		if(!window.hasFocus() && cfg.saveEnergy == true){
			window.setFramerateLimit(15);
		} else {
			window.setFramerateLimit(winFPS);
		}

		//Change progress bar time
		progressBarTime.setString(toHumanTime(time) + "/" + toHumanTime(duration));
		bounds = progressBarTime.getLocalBounds();
		progressBarTime.setPosition(winWidth - 10 - bounds.width, winHeight - 42);

		//Set bars values
		BASS_ChannelGetData(channel, fft, fftfreq);
		for (int a = 0; a<barAmount; a++){
			float sum = 0;
			for (int j = 0; j<smoothBy - 1; j++){
				smoothingBars[a][j] = smoothingBars[a][j + 1];
				sum += smoothingBars[a][j];
			}
			smoothingBars[a][smoothBy - 1] = fft[a];
			sum += smoothingBars[a][smoothBy - 1];
			bars[a] = sqrt(sum / smoothBy)*-600 - 1;
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

		//Draw everything on window
		window.clear(winBackground);
		window.draw(sprite,&shader);
		window.draw(text);
		for (int i = 0; i<barAmount; i++){
			barRect[i].setSize(sf::Vector2f(10, static_cast<int>(aveBars[i])));
			window.draw(barRect[i]);
		}
		window.draw(progressBarBack);
		window.draw(progressBarFront);
		window.draw(progressBarTime);
		for(int i=0; i<buttonList.size(); i++){
			buttonList[i].draw(window);
		}
		window.display();
	}

	system("pause");
	return 0;
}