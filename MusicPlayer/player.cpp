// SFML
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
// Boost
#include <boost/filesystem.hpp>
// Standard
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <fstream>
#include <shlobj.h>
#include <windows.h>
#include <cctype> // toupper
// TagLib
#include <mpegfile.h>
#include <id3v1tag.h>
#include <id3v2tag.h>
#include <attachedpictureframe.h>
// Bass
#include "bass.h"
// Project related
#include "logger.hpp"
#include "config.hpp"
#include "player.hpp"
#include "spectrumComponent.hpp"
#include "button.hpp"
#include "song.hpp"
#include "GUI/window.hpp"

namespace Player{
	/*
		###############################################
			Variables
		###############################################
	*/

	sf::RenderWindow window(sf::VideoMode(cfg.winWidth, cfg.winHeight), cfg.winTitle);
	std::vector<std::wstring> tracks; // Initialize tracks vector, which will hold track paths
	size_t trackNow = 0; // Index of track used in tracks vector
	bool isSongPlaying = true; // Is stream playing?
	
	song songNow;

	// Screen related things
	SpectrumComp spectrumComp;


	/*
		###############################################
			Player Functions
		###############################################
	*/

	void defaultAlbum(){
		spectrumComp.texture = cfg.spr_blankAlbum;
		float ratio = (cfg.winWidth>cfg.winHeight) ? (cfg.winWidth/spectrumComp.albumCoverSprite.getLocalBounds().width) : (cfg.winHeight/spectrumComp.albumCoverSprite.getLocalBounds().height);
		spectrumComp.albumCoverSprite.setTexture(spectrumComp.texture);
		spectrumComp.albumCoverSprite.setPosition(sf::Vector2f(static_cast<float>(cfg.winWidth/2), static_cast<float>(cfg.winHeight/2)));
		spectrumComp.albumCoverSprite.setOrigin(sf::Vector2f(spectrumComp.albumCoverSprite.getLocalBounds().width/2, spectrumComp.albumCoverSprite.getLocalBounds().height/2));
		spectrumComp.albumCoverSprite.setScale(sf::Vector2f(ratio, ratio));
		spectrumComp.albumCover.setTexture(&spectrumComp.texture);
		spectrumComp.albumCover.setTextureRect(sf::IntRect(0, 0, spectrumComp.texture.getSize().x, spectrumComp.texture.getSize().y));
	}

	// Set album cover on the back
	void refreshAlbum(){
		if(!tracks.empty()){
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
						TagLib::ID3v2::AttachedPictureFrame *picFrame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(*i);
						unsigned long picSize = picFrame->picture().size();
						void* outImage = malloc(picSize);
						if(outImage){
							// Copy to allocated memory whole picture data
							memcpy(outImage, picFrame->picture().data(), picSize);

							// Load texture
							if(!spectrumComp.texture.loadFromMemory(outImage,picSize)){
								std::cout << "problem?";
							} else {
								// Set albumCoverSprite texture and position it on window
								float ratio = (cfg.winWidth>cfg.winHeight) ? (cfg.winWidth/spectrumComp.albumCoverSprite.getLocalBounds().width) : (cfg.winHeight/spectrumComp.albumCoverSprite.getLocalBounds().height);
								spectrumComp.albumCoverSprite.setTexture(spectrumComp.texture);
								spectrumComp.albumCoverSprite.setPosition(sf::Vector2f(static_cast<float>(cfg.winWidth/2), static_cast<float>(cfg.winHeight/2)));
								spectrumComp.albumCoverSprite.setOrigin(sf::Vector2f(spectrumComp.albumCoverSprite.getLocalBounds().width/2, spectrumComp.albumCoverSprite.getLocalBounds().height/2));
								spectrumComp.albumCoverSprite.setScale(sf::Vector2f(ratio, ratio));
								spectrumComp.albumCover.setTexture(&spectrumComp.texture);
								spectrumComp.albumCover.setTextureRect(sf::IntRect(0, 0, spectrumComp.texture.getSize().x, spectrumComp.texture.getSize().y));
							}
						}
						free(outImage);
						//delete picFrame;
						break;
					}
				} else {
					defaultAlbum();
				}
			} else {
				defaultAlbum();
			}
		}
	}

	// Change channel
	void changeChannel(unsigned short a_whichOne){
		BASS_ChannelPause(cfg.channel);
		if(!BASS_SetDevice(a_whichOne)){
			BASS_Init(a_whichOne, cfg.freq, NULL, window.getSystemHandle(), NULL);
		}
		BASS_ChannelSetDevice(cfg.channel, a_whichOne);
		BASS_ChannelPlay(cfg.channel, false);
	}

	void fontReload(){
		spectrumComp.fontReload();
	}

	/*
		###############################################
			Player States
		###############################################
	*/

	// Play song
	void playTrack(){
		if(!tracks.empty()){
			// Free channel data
			try{
				if(cfg.channel != NULL && !BASS_StreamFree(cfg.channel)){
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
			std::wstring toTitle = tracks[trackNow];
			size_t lastSlash   = toTitle.find_last_of(L'\\');
			size_t lastDot     = toTitle.find_last_of(L'.');
			size_t lastHyphen  = toTitle.find_last_of(L'-');
			songNow.artist = tracks[trackNow].substr(lastSlash+1, lastHyphen-lastSlash-1);
			for(size_t i=0; i<songNow.artist.size(); i++){
				songNow.artist[i] = std::toupper(songNow.artist[i]);
			}
			songNow.title = tracks[trackNow].substr(lastHyphen+2, lastDot-lastHyphen-2);
	
			// Set author name to uppercase and save it to "title" var
			spectrumComp.setAuthorAndTitle(songNow);

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
			cfg.channel = BASS_StreamCreateFile(FALSE, tracks[trackNow].c_str(), 0, 0, 0);
			BASS_ChannelSetAttribute(cfg.channel, BASS_ATTRIB_VOL, 0.3f);
			BASS_ChannelPlay(cfg.channel, FALSE);
			isSongPlaying = true;

			spectrumComp.onSongUpdate(BASS_ChannelBytes2Seconds(cfg.channel, BASS_ChannelGetLength(cfg.channel, BASS_POS_BYTE)));

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
			BASS_ChannelPause(cfg.channel);
			isSongPlaying = !isSongPlaying;
			Logger::log("INFO - player.cpp pauseSong()", "Pausing song");
		} else {
			BASS_ChannelPlay(cfg.channel, FALSE);
			isSongPlaying = !isSongPlaying;
			Logger::log("INFO - player.cpp pauseSong()", "Resuming song");
		}
	}

	/*
		###############################################
			Tracks-related Functions
		###############################################
	*/

	// Shuffle tracks that are passed by referenced vector
	void shuffleTracks(std::vector<std::wstring> &a_tracks){
		std::shuffle(a_tracks.begin(), a_tracks.end(), std::mt19937(static_cast<unsigned int>(time(nullptr))));
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
				Logger::log("INFO - player.cpp dialogHandler()", "Dialog initialized");
				break;
			case BFFM_SELCHANGED:
				TCHAR path[MAX_PATH];
				SHGetPathFromIDList((LPITEMIDLIST)(lParam), path);
				Logger::log("DEBUG - player.cpp dialogHandler() - path selected", path);
				break;
			default:
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
		if(a_dir.empty()){
			// Stop, free and clear all variables used for playing
			isSongPlaying = false;
			BASS_ChannelStop(cfg.channel);
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
				BASS_ChannelStop(cfg.channel);
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


	// Init function
	void init(){
		// Initialize Bass
		if(!BASS_Init(cfg.deviceID, cfg.freq, NULL, window.getSystemHandle(), nullptr)){
			Logger::log("ERROR - player.cpp init()", "Number of bass error: " + std::to_string(BASS_ErrorGetCode()));
		};

		// Apply window settings
		cfg.setWindowSettings(window);
		Logger::log("INFO - player.cpp init()", "Set window settings");

		// Reload components
		spectrumComp.reload();
	}

	// Draw window
	void draw(){
		while(window.isOpen()){
			switch(Screens::screenOnUse){
				//# VISUALIZER #
				case View::Spectrum:
					spectrumComp.draw(window);
					break;

				//# DEFAULT #
				default:
				case View::None:
					break;
			}
			
			// Check state of channel
			switch(BASS_ChannelIsActive(cfg.channel)){
				case BASS_ACTIVE_PLAYING: // PLAYING STATE
					break;
				case BASS_ACTIVE_PAUSED:  // PAUSED STATE
					break;
				case BASS_ACTIVE_STOPPED: // STOPPED OR FINISHED STATE
					if(!tracks.empty()){
						playNext();
					}
					break;
				default:
					break;
			}			
		}
	}
}