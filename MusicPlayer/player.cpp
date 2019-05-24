// SFML
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
// Standard
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
// Boost
#include <boost/filesystem.hpp>
// Bass
#include "bass.h"
// TagLib
#include <tag.h>
#include <fileref.h>
#include <mpegfile.h>
#include <id3v1tag.h>
#include <id3v2tag.h>
#include <id3v2frame.h>
#include <id3v2header.h>
#include <attachedpictureframe.h>
// Project related
#include "config.h"
#include "mymisc.h"
#include "button.h"
#include "spectrumComponent.h"
#include "player.h"
#include "logger.h"

namespace Player {
	/*
		###############################################
			Variables
		###############################################
	*/

	SpectrumComp spectrumComp;        // Spectrum component
	HSTREAM channel = NULL;           // Bass channel
	sf::Color winBackground;          // Window background
	std::vector<std::wstring> tracks; // Initialize tracks vector, which will hold track paths
	int trackNow = 0;                 // Index of track used in tracks vector
	bool isSongPlaying = true;            // Is stream playing?
	sf::RenderWindow window(sf::VideoMode(cfg.winWidth, cfg.winHeight), cfg.winTitle);
	sf::WindowHandle hwnd = window.getSystemHandle();
	WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };               // Get window placement
	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);                // Get window style
	bool isClickedWindow = false;
	std::vector<GUI::Button> buttonListOfDevices = {};
	std::vector<GUI::wButton> buttonList = {
		GUI::wButton(L"Previous song", GUI::POS_NORMAL, sf::Vector2f(10.0f, 90.0f), sf::Vector2f(5.0f, 5.0f), cfg.lighter_grey),
		GUI::wButton(L"Pause song", GUI::POS_NORMAL, sf::Vector2f(10.0f, 60.0f), sf::Vector2f(5.0f, 5.0f), cfg.lighter_grey),
		GUI::wButton(L"Next song", GUI::POS_NORMAL, sf::Vector2f(10.0f, 30.0f), sf::Vector2f(5.0f, 5.0f), cfg.lighter_grey),
		GUI::wButton(L"Open Folder", GUI::POS_BOTTOM, sf::Vector2f(10.0f, 120.0f), sf::Vector2f(5.0f, 5.0f), cfg.lighter_grey)
	};

	/*
		###############################################
			Window-related Functions
		###############################################
	*/

	// Change window fullscreen or not
	void windowSetFullscreen(sf::WindowHandle a_handle){
		if(cfg.isFullscreen == false){
			// "mi" will store monitor information, which will be used for positioning
			MONITORINFO mi = { sizeof(mi) };
			if(GetWindowPlacement(a_handle, &g_wpPrev) && GetMonitorInfo(MonitorFromWindow(a_handle, MONITOR_DEFAULTTOPRIMARY), &mi)){
				// Set window to fullscreen
				SetWindowLong(a_handle, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(a_handle, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
			}
			cfg.isFullscreen = true;
		} else {
			// Set window to windowed
			SetWindowLong(a_handle, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
			SetWindowPlacement(a_handle, &g_wpPrev);
			SetWindowPos(a_handle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
			cfg.isFullscreen = false;
		}
	}

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
						void *outImage = malloc(picSize);
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
								spectrumComp.albumCoverSprite.setPosition(sf::Vector2f(cfg.winWidth/2, cfg.winHeight/2));
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
					}
				}
			}
		}
	}

	// Change channel
	void changeChannel(unsigned short a_whichOne){
		BASS_ChannelPause(channel);
		if(!BASS_SetDevice(a_whichOne)){
			BASS_Init(a_whichOne, cfg.freq, NULL, hwnd, NULL);
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
			if (trackNow + 1 >= tracks.size()){
				std::cout << "Next: " << static_cast<boost::filesystem::path>(tracks[0]).filename() << "\n\n\n\n";
			} else {
				std::cout << "Next: " << static_cast<boost::filesystem::path>(tracks[trackNow+1]).filename() << "\n\n\n\n";
			}

			// Find title and author by pattern "Author - Title" in music filename
			std::wstring toTitle    = tracks[trackNow];
			std::size_t lastSlash   = toTitle.find_last_of(L"/\\");
			std::size_t lastDot     = toTitle.find_last_of(L".");
			std::size_t lastHyphen  = toTitle.find_last_of(L"-");
			spectrumComp.setAuthor(tracks[trackNow].substr(lastSlash+1,lastHyphen-lastSlash-1));
	
			// Set author name to uppercase and save it to "title" var
			spectrumComp.setTitle(tracks[trackNow].substr(lastHyphen+2,lastDot-lastHyphen-2));

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
		logSys.log("INFO - player.cpp playNext()", "Playing next track #" + intToString(trackNow));

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
		logSys.log("INFO - player.cpp playPrevious()", "Playing previous track #" + intToString(trackNow));

		// Play track
		playTrack();
	}

	// Pause or play song
	void pauseSong(){
		// If song is playing [then STOP]
		if(isSongPlaying){
			BASS_ChannelPause(channel);
			isSongPlaying = !isSongPlaying;
			logSys.log("INFO - player.cpp pauseSong()", "Pausing song");
			changePlayButtonIcon(1);
		} else {
			BASS_ChannelPlay(channel, FALSE);
			isSongPlaying = !isSongPlaying;
			logSys.log("INFO - player.cpp pauseSong()", "Resuming song");
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
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> uni(0, a_tracks.size()-1);
		for (int i = 0; i < a_tracks.size(); i++){
			std::swap(a_tracks[i], a_tracks[uni(rng)]);
		}
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
				logSys.log("INFO - player.cpp dialogHandler()", "Dialog initalized");
				break;
			case BFFM_SELCHANGED:
				TCHAR path[MAX_PATH];
				SHGetPathFromIDList((LPITEMIDLIST)lParam, path);
				logSys.log("DEBUG - player.cpp dialogHandler() - path selected", path);
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
			logSys.log("INFO - player.cpp takeMusicFromFolder()", "Freed memory and stopped playing song");
			
			// Take music from working directory, then shuffleTracks and play it
			tracks = takeMusic(boost::filesystem::current_path());
			shuffleTracks(tracks);
			logSys.log("INFO - player.cpp takeMusicFromFolder()", "shuffleTracksd songs");

			// Play first track
			playTrack();
			logSys.log("INFO - player.cpp takeMusicFromFolder()", "Playing track");

			// [DEBUG] Print how many songs are in this folder
			logSys.log("DEBUG - player.cpp takeMusicFromFolder()", "Amount of songs: " + intToString(tracks.size()));	
		} else {
			// Open file browsing dialog and choose folder
			std::string path = browseFilesDialog(a_dir);
			if(path != ""){
				// Stop, free and clear all variables used for playing
				isSongPlaying = false;
				BASS_ChannelStop(channel);
				tracks.clear();
				trackNow = 0;
				logSys.log("INFO - player.cpp takeMusicFromFolder()", "Freed memory and stopped playing song");

				// Take music from path we chose, then shuffleTracks and play it
				tracks = takeMusic((boost::filesystem::path) path);
				shuffleTracks(tracks);
				logSys.log("INFO - player.cpp takeMusicFromFolder()", "shuffleTracksd songs");

				// Play first track
				playTrack();
				logSys.log("INFO - player.cpp takeMusicFromFolder()", "Playing track");

				// [DEBUG] Print how many songs are in this folder
				logSys.log("DEBUG - player.cpp takeMusicFromFolder()", "Amount of songs: " + intToString(tracks.size()));
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
			if(!BASS_Init(cfg.deviceID, cfg.freq, NULL, hwnd, NULL)){
				throw BASS_ErrorGetCode();
			};
			logSys.log("INFO - player.cpp init()", "Initalized bass");
		} catch(int e){
			logSys.log("ERROR - player.cpp init()", "Number of bass error: " + intToString(e));
		} catch(...){
			logSys.log("ERROR - player.cpp init()", "Unexpected error!");
		}

		// Apply window settings
		window.setVerticalSyncEnabled(false);
		window.setFramerateLimit(144);
		window.setKeyRepeatEnabled(false);
		winBackground = sf::Color::Color(123, 123, 123);
		logSys.log("INFO - player.cpp init()", "Set window settings");

		// Set shader variables
		cfg.shader_brightness.setUniform("blur_radius", sf::Vector2f(0.003f, 0.003f));
		cfg.shader_glass.setUniform("blur_radius", sf::Vector2f(0.001f, 0.001f));
		logSys.log("INFO - player.cpp init()", "Updated shaders settings");

		// Push buttons
		BASS_DEVICEINFO info;
		for(int a = 1; BASS_GetDeviceInfo(a, &info); a++){
			buttonListOfDevices.push_back(GUI::Button((std::string)info.name, GUI::POS_BOTTOM, sf::Vector2f(10.0f, 120.0+a*30.0f), sf::Vector2f(5.0f, 5.0f), cfg.lighter_grey));
			buttonListOfDevices[a-1].buttonValue = a;
		}
		logSys.log("INFO - player.cpp init()", "Appended buttons");


		takeMusicFromFolder();
	}
	
	// Draw window
	void draw(){
		while(window.isOpen()){
			// Get time and duration of track and set size of prograss bar
			spectrumComp.updateProgressBar(sf::Vector2f((cfg.winWidth - 20)*BASS_ChannelGetPosition(channel, BASS_POS_BYTE) / BASS_ChannelGetLength(channel, BASS_POS_BYTE), 6));
		
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
			spectrumComp.updateProgressBarTime(channel);

			//######################################################### VISUALIZER

			// Take channel data, change it into visualizer bar data and smooth each bar
			spectrumComp.updateVisualizerBars(channel);

			//######################################################### WINDOW EVENTS

			sf::Event event;
			while(window.pollEvent(event)){
				switch (event.type){
					// On window close
					case sf::Event::Closed:
						window.close();
						break;
					// On window resize
					case sf::Event::Resized:
						window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
						spectrumComp.onWindowResizing(event.size.width, event.size.height);
						for(int i=0; i<buttonList.size(); i++){
							buttonList[i].update();
						}
						for(int i=0; i<buttonListOfDevices.size(); i++){
							buttonListOfDevices[i].update();
						}
						refreshAlbum();
						break;
					// On key press
					case sf::Event::KeyPressed:
						if(event.key.code == sf::Keyboard::F11){
							windowSetFullscreen(hwnd);
						} else 
						if(event.key.code == sf::Keyboard::H){
							cfg.drawHUD = !cfg.drawHUD;
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
			spectrumComp.updateAuthorAndTitle();

			// Draw everything on the window
			window.clear(winBackground);
			spectrumComp.draw(window);
			if(cfg.drawHUD){
				for(int i=0; i<buttonList.size(); i++){
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