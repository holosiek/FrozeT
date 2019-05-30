// SFML
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
// Boost
#include <boost/filesystem.hpp>
// Standard
#include <algorithm>
#include <iostream>
#include <string>
#include <math.h>
#include <cmath>
#include <vector>
#include <random>
#include <fstream>
#include <shlobj.h>
#include <windows.h>
#include <sstream>
#include <cctype> // toupper
// TagLib
#include <tag.h>
#include <fileref.h>
#include <mpegfile.h>
#include <id3v1tag.h>
#include <id3v2tag.h>
#include <id3v2frame.h>
#include <id3v2header.h>
#include <attachedpictureframe.h>
// Bass
#include "bass.h"
// Project related
#include "logger.hpp"
#include "config.hpp"
#include "player.hpp"
#include "spectrumComponent.hpp"
#include "button.hpp"
#include "mymisc.hpp"
#include "song.hpp"
#include "GUI/window.hpp"
#include "GUI/screen.hpp"

namespace Player{
	/*
		###############################################
			Variables
		###############################################
	*/

	HSTREAM channel = NULL;           // Bass channel
	std::vector<std::wstring> tracks; // Initialize tracks vector, which will hold track paths
	size_t trackNow = 0;                 // Index of track used in tracks vector
	bool isSongPlaying = true;            // Is stream playing?
	sf::RenderWindow window(sf::VideoMode(cfg.winWidth, cfg.winHeight), cfg.winTitle);
	
	bool isClickedWindow = false;
	std::vector<GUI::Button> buttonListOfDevices = {};
	std::vector<GUI::wButton> buttonList = {};
	song songNow;
	// Screen related things
	enum Screens{
		None,
		Spectrum,
		Tracklist,
		Settings
	};
	SpectrumComp spectrumComp;
	Screens screenOnUse = Screens::Spectrum;

	/*
		###############################################
			Window-related Functions
		###############################################
	*/
	

	// Set if program should go in safe mode [less GPU usege]
	void saveMode(bool a_state){
		if(a_state && cfg.saveEnergy){
			window.setFramerateLimit(15);
		} else {
			window.setFramerateLimit(cfg.winFPS);
		}
	}

	// Check if window is clicked
	void onWindowClick(bool a_check){
		isClickedWindow = a_check;
	}

	// Change button icon
	void changePlayButtonIcon(int a_state){
		if(a_state == 1){
			buttonList[2].setIcon(cfg.spr_play_button);
		} else {
			buttonList[2].setIcon(cfg.spr_pause_button);
		}
	}

	/*
		###############################################
			Player Functions
		###############################################
	*/

	// Set album cover on the back
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
						void* outImage = malloc(picSize);
						if (outImage){
							// Copy to allocated memory whole picture data
							memcpy(outImage, picFrame->picture().data(), picSize);

							// Load texture
							if(!spectrumComp.texture.loadFromMemory(outImage,picSize)){
								std::cout << "problem?";
							} else {
								// Set albumCoverSprite texture and position it on window
								float ratio;
								spectrumComp.albumCoverSprite.setTexture(spectrumComp.texture);
								spectrumComp.albumCoverSprite.setPosition(sf::Vector2f(static_cast<float>(cfg.winWidth/2), static_cast<float>(cfg.winHeight/2)));
								spectrumComp.texture.setSmooth(true);
								if(cfg.winWidth>cfg.winHeight){
									ratio = cfg.winWidth/spectrumComp.albumCoverSprite.getLocalBounds().width;
								} else {
									ratio = cfg.winHeight/spectrumComp.albumCoverSprite.getLocalBounds().height;
								}
								spectrumComp.albumCoverSprite.setOrigin(sf::Vector2f(spectrumComp.albumCoverSprite.getLocalBounds().width/2,spectrumComp.albumCoverSprite.getLocalBounds().height/2));
								spectrumComp.albumCoverSprite.setScale(sf::Vector2f(ratio,ratio));
								spectrumComp.albumCover.setTexture(&spectrumComp.texture);
								spectrumComp.albumCover.setTextureRect(sf::IntRect(0,0,spectrumComp.albumCover.getTexture()->getSize().x,spectrumComp.albumCover.getTexture()->getSize().y));
							}
						}
						free(outImage);
					}
				}
			}
		}
	}

	// Change channel
	void changeChannel(unsigned short a_whichOne){
		BASS_ChannelPause(channel);
		if(!BASS_SetDevice(a_whichOne)){
			BASS_Init(a_whichOne, cfg.freq, NULL, window.getSystemHandle(), NULL);
		}
		BASS_ChannelSetDevice(channel, a_whichOne);
		BASS_ChannelPlay(channel, false);
	}

	/*
		###############################################
			Player States
		###############################################
	*/

	// Play song
	void playTrack(){
		if(tracks.size() != 0){
			// Free channel data
			try{
				if(channel != NULL && !BASS_StreamFree(channel)){
					throw BASS_ErrorGetCode();
				};
			} catch(int e){
				std::cout << "[Bass Error] [player.cpp playTrack()] Number of error: " << e;
			} catch(...){
				std::cout << "Unexpected error!";
			}

			// Diplay in console "Playing now"
			std::cout << "-------------------------\nPlaying now: " << static_cast<boost::filesystem::path>(tracks[trackNow]).filename() << std::endl;
			if(trackNow + 1 >= tracks.size()){
				std::cout << "Next: " << static_cast<boost::filesystem::path>(tracks[0]).filename() << "\n\n\n\n";
			} else {
				std::cout << "Next: " << static_cast<boost::filesystem::path>(tracks[trackNow+1]).filename() << "\n\n\n\n";
			}

			// Find title and author by pattern "Author - Title" in music filename
			std::wstring toTitle    = tracks[trackNow];
			size_t lastSlash   = toTitle.find_last_of(L"/\\");
			size_t lastDot     = toTitle.find_last_of(L".");
			size_t lastHyphen  = toTitle.find_last_of(L"-");
			songNow.artist = tracks[trackNow].substr(lastSlash+1,lastHyphen-lastSlash-1);
			for(size_t i=0; i<songNow.artist.size(); i++){
				songNow.artist[i] = std::toupper(songNow.artist[i]);
			}
			songNow.title = tracks[trackNow].substr(lastHyphen+2,lastDot-lastHyphen-2);
	
			// Set author name to uppercase and save it to "title" var
			spectrumComp.updateAuthorAndTitle(songNow);

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
			isSongPlaying = true;

			spectrumComp.onSongUpdate(BASS_ChannelBytes2Seconds(channel, BASS_ChannelGetLength(channel, BASS_POS_BYTE)));

			// Load album image
			refreshAlbum();
		}
	}	

	// Play next track
	void playNext(){
		// Set new track number
		trackNow++;
		if(trackNow >= tracks.size()){
			trackNow = 0;
		}
		Logger::log("INFO - player.cpp playNext()", "Playing next track");

		// Play track
		playTrack();
	}

	// Play previous track
	void playPrevious(){
		// Set new track number
		trackNow--;
		if(trackNow < 0){
			trackNow = tracks.size()-1;
		}
		Logger::log("INFO - player.cpp playPrevious()", "Playing previous track");

		// Play track
		playTrack();
	}

	// Pause or play song
	void pauseSong(){
		// If song is playing [then STOP]
		if(isSongPlaying){
			BASS_ChannelPause(channel);
			isSongPlaying = !isSongPlaying;
			Logger::log("INFO - player.cpp pauseSong()", "Pausing song");
			changePlayButtonIcon(1);
		} else {
			BASS_ChannelPlay(channel, FALSE);
			isSongPlaying = !isSongPlaying;
			Logger::log("INFO - player.cpp pauseSong()", "Resuming song");
			changePlayButtonIcon(0);
		}
	}

	/*
		###############################################
			Tracks-related Functions
		###############################################
	*/

	// Shuffle tracks that are passed by referenced vector
	void shuffleTracks(std::vector<std::wstring> &a_tracks){
		std::shuffle(a_tracks.begin(), a_tracks.end(), std::mt19937(static_cast<unsigned int>(time(NULL))));
	}

	/*
		###############################################
			Folder Functions
		###############################################
	*/

	// Handles our file system dialog
	int CALLBACK dialogHandler(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData){
		// [DEBUG] Check whenever selection in dialog changed
		switch(uMsg){
			case BFFM_INITIALIZED:
				Logger::log("INFO - player.cpp dialogHandler()", "Dialog initalized");
				break;
			case BFFM_SELCHANGED:
				TCHAR path[MAX_PATH];
				SHGetPathFromIDList((LPITEMIDLIST)lParam, path);
				Logger::log("DEBUG - player.cpp dialogHandler() - path selected", path);
				break;
		}
		return 0;
	}

	// Change std::wstring to std::string [COPY-PASTE TO CHECK]
	std::string UTF16ToUTF8(const std::wstring &s){
		const int size = ::WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, 0, NULL);
		std::vector<char> buf(size);
		::WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, &buf[0], size, 0, NULL);

		return std::string(&buf[0]);
	}

	// Show file system dialog
	std::string browseFilesDialog(std::wstring const a_dialogTitle = L""){
		// Create buffer for path name
		TCHAR path[MAX_PATH];

		// Prepare BROWSEINFO struct for dialog settings
		BROWSEINFO dialogOptions            = { NULL };
				   dialogOptions.lpszTitle  = UTF16ToUTF8(a_dialogTitle).c_str();
				   dialogOptions.ulFlags    = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON;
				   dialogOptions.lpfn       = dialogHandler;
				   dialogOptions.lParam     = NULL;
		LPITEMIDLIST pidl = SHBrowseForFolder(&dialogOptions);

		// If file system dialog is not canceled
		if(pidl != NULL){
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

	// Search for mp3 in folder
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

	// Choose folder method
	void takeMusicFromFolder(const std::wstring a_dir = L""){
		if(a_dir == L""){
			// Stop, free and clear all variables used for playing
			isSongPlaying = false;
			BASS_ChannelStop(channel);
			tracks.clear();
			trackNow = 0;
			Logger::log("INFO - player.cpp takeMusicFromFolder()", "Freed memory and stopped playing song");
			
			// Take music from working directory, then shuffleTracks and play it
			tracks = takeMusic(boost::filesystem::current_path());
			shuffleTracks(tracks);
			Logger::log("INFO - player.cpp takeMusicFromFolder()", "shuffled tracks");

			// Play first track
			playTrack();
			Logger::log("INFO - player.cpp takeMusicFromFolder()", "Playing track");

			// [DEBUG] Print how many songs are in this folder
			if(cfg.debugMode){
				Logger::log("DEBUG - player.cpp takeMusicFromFolder()", "Amount of songs: " + std::to_string(tracks.size()));	
			}
		} else {
			// Open file browsing dialog and choose folder
			std::string path = browseFilesDialog(a_dir);
			if(path != ""){
				// Stop, free and clear all variables used for playing
				isSongPlaying = false;
				BASS_ChannelStop(channel);
				tracks.clear();
				trackNow = 0;
				Logger::log("INFO - player.cpp takeMusicFromFolder()", "Freed memory and stopped playing song");

				// Take music from path we chose, then shuffleTracks and play it
				tracks = takeMusic((boost::filesystem::path) path);
				shuffleTracks(tracks);
				Logger::log("INFO - player.cpp takeMusicFromFolder()", "shuffled tracks");

				// Play first track
				playTrack();
				Logger::log("INFO - player.cpp takeMusicFromFolder()", "Playing track");

				// [DEBUG] Print how many songs are in this folder
				if(cfg.debugMode){
					Logger::log("DEBUG - player.cpp takeMusicFromFolder()", "Amount of songs: " + std::to_string(tracks.size()));
				}
			}
		}
	}

	/*
		###############################################
			Main Functions
		###############################################
	*/

	// Init function [It's almost first to execute]
	void init(){
		// Initialize Bass
		try{
			if(!BASS_Init(cfg.deviceID, cfg.freq, NULL, window.getSystemHandle(), NULL)){
				throw BASS_ErrorGetCode();
			};
			Logger::log("INFO - player.cpp init()", "Initalized bass");
		} catch(int e){
			Logger::log("ERROR - player.cpp init()", "Number of bass error: " + std::to_string(e));
		} catch(...){
			Logger::log("ERROR - player.cpp init()", "Unexpected error!");
		}

		// Apply window settings
		cfg.setWindowSettings(window);
		Logger::log("INFO - player.cpp init()", "Set window settings");

		// Push buttons
		BASS_DEVICEINFO info;
		for(int a = 1; BASS_GetDeviceInfo(a, &info); a++){
			buttonListOfDevices.push_back(GUI::Button((std::string)info.name, GUI::POS_BOTTOM, sf::Vector2f(10.0f, 120.0f+a*30.0f), sf::Vector2f(5.0f, 5.0f), cfg.lighter_grey));
			buttonListOfDevices[a-1].buttonValue = a;
		}
		Logger::log("INFO - player.cpp init()", "Appended buttons");


		// Create buttons
		std::vector<GUI::wButton> but = {
			GUI::wButton(L"Previous song", GUI::POS_NORMAL, sf::Vector2f(10.0f, 90.0f), sf::Vector2f(5.0f, 5.0f), cfg.lighter_grey),
			GUI::wButton(L"Pause song", GUI::POS_NORMAL, sf::Vector2f(10.0f, 60.0f), sf::Vector2f(5.0f, 5.0f), cfg.lighter_grey),
			GUI::wButton(L"Next song", GUI::POS_NORMAL, sf::Vector2f(10.0f, 30.0f), sf::Vector2f(5.0f, 5.0f), cfg.lighter_grey),
			GUI::wButton(L"Open Folder", GUI::POS_BOTTOM, sf::Vector2f(10.0f, 120.0f), sf::Vector2f(5.0f, 5.0f), cfg.lighter_grey)
		};
		buttonList = std::move(but);

		takeMusicFromFolder();
	}
	
	// Draw window
	void draw(){
		while(window.isOpen()){
			// Clear window
			window.clear(cfg.winBackground);

			// Get time and duration of track and set size of prograss bar
			spectrumComp.updateProgressBar(sf::Vector2f(static_cast<float>((cfg.winWidth - 20)*BASS_ChannelGetPosition(channel, BASS_POS_BYTE) / BASS_ChannelGetLength(channel, BASS_POS_BYTE)), 6.0f));
		
			// Check state of channel
			switch(BASS_ChannelIsActive(channel)){
				case BASS_ACTIVE_PLAYING: // PLAYING STATE
					break;
				case BASS_ACTIVE_PAUSED:  // PAUSED STATE
					break;
				default:
					if(tracks.size() != 0){
						playNext();
					}
					break;
			}
			//######################################################### VISUALIZER
			
			// Change progress bar text and position
			spectrumComp.updateProgressBarTime(channel);

			// Take channel data, change it into visualizer bar data and smooth each bar
			spectrumComp.updateVisualizerBars(channel);

			//######################################################### WINDOW EVENTS

			sf::Event event;
			while(window.pollEvent(event)){
				switch(event.type){
					// On window close
					case sf::Event::Closed:
						window.close();
						break;
					// On window resize
					case sf::Event::Resized:
						window.setView(sf::View(sf::FloatRect(0.0f, 0.0f, event.size.width, event.size.height)));
						spectrumComp.onWindowResizing(event.size.width, event.size.height);
						for(size_t i=0; i<buttonList.size(); i++){
							buttonList[i].update();
						}
						for(size_t i=0; i<buttonListOfDevices.size(); i++){
							buttonListOfDevices[i].update();
						}
						refreshAlbum();
						break;
					// On key press
					case sf::Event::KeyPressed:
						if(event.key.code == sf::Keyboard::F11){
							GUI::Window::windowSetFullscreen(window.getSystemHandle());
						} else 
						if(event.key.code == sf::Keyboard::H){
							cfg.drawHUD = !cfg.drawHUD;
						}
						if(event.key.code == sf::Keyboard::Space){
							pauseSong();
						}
						break;
					// On mouse press
					case sf::Event::MouseButtonPressed:
						if(event.mouseButton.button == 0){
							sf::Vector2i mousePos(event.mouseButton.x,event.mouseButton.y);
							if(buttonList[0].isClicked(mousePos)){
								playPrevious();
							}
							if(buttonList[1].isClicked(mousePos)){
								pauseSong();
							}
							if(buttonList[2].isClicked(mousePos)){
								playNext();
							}
							if(buttonList[3].isClicked(mousePos)){
								takeMusicFromFolder(L"Open folder which holds music");
							}
							for(GUI::Button button : buttonListOfDevices){
								if(button.isClicked(mousePos)){
									changeChannel(button.buttonValue);
								}
							}
						}
						break;
					case sf::Event::MouseEntered:
						onWindowClick(true);
						break;
					case sf::Event::MouseLeft:
						onWindowClick(false);
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

			if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && isClickedWindow){
				sf::Vector2i mouseCords = sf::Mouse::getPosition(window);
				double getClicked = spectrumComp.onClickProgressBar(sf::Vector2i(mouseCords.x,mouseCords.y));
				if(getClicked != -1){
					BASS_ChannelSetPosition(channel, BASS_ChannelSeconds2Bytes(channel, getClicked), BASS_POS_BYTE);
				}
			}

			// Set artist and song title text

			// Draw everything on the window
			
			switch(screenOnUse){
				case Screens::Spectrum:
					spectrumComp.draw(window);
					break;
				default:
				case Screens::None:
					break;
			}
			if(cfg.drawHUD){
				for(size_t i=0; i<buttonList.size(); i++){
					buttonList[i].draw(window);
				}
				for(GUI::Button button : buttonListOfDevices){
					button.draw(window);
				}
			}
		
			window.display();
		}
	}
}