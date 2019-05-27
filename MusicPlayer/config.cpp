// SFML
#include <SFML/Graphics.hpp>
// Standard
#include <Windows.h>
#include <string>
#include <fstream>
// Bass
#include "bass.h"
// JSON
#include <nlohmann/json.hpp>
// Project related
#include "logger.hpp"
#include "config.hpp"

// Transform relative path to executable path [Used cuz of "default program" system]
std::string relPathToExePath(std::string a_relPath){
	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);
	std::string::size_type pos = std::string(path).find_last_of("\\/");
	std::string pathDir = std::string(path).substr(0, pos)+"\\";
	return pathDir + a_relPath;
}

/*
	########################################
	Config loading functions
*/

// Save to variables config options
void Config::loadSettings(nlohmann::json &a_json){
	try{
		// Main and debug variables
		programVersion  = a_json["version"];
		debugMode       = a_json["debug_mode"];

		// Settings
		saveEnergy      = a_json["save_energy_mode"];
		saveTitleToFile = a_json["save_title_to_file"];

		// Channel
		deviceID        = a_json["playback_device_id"];
		freq            = a_json["channel_frequency"];
		fftfreq         = a_json["fft_frequency"];

		// Window
		winWidth        = a_json["window_width"];
		winHeight       = a_json["window_height"];
		winFPS          = a_json["fps"];
		winTitle        = a_json["window_title"];
		isFullscreen    = a_json["window_fullscreen_mode"];
		drawHUD         = a_json["draw_hud"];

		// Fonts
		s_fBold         = a_json["primary_font"];
		s_fRegular      = a_json["secondary_font"];
		s_fNormal       = a_json["menu_font"];
	} catch(std::exception &e){
		logSys.log("ERROR - config.cpp loadSettings()", e.what());
	} catch(...){
		logSys.log("ERROR - config.cpp loadSettings()", "Unexpected error!");
	}
	logSys.log("INFO - config.cpp loadSettings()", "Loaded settings!");
}

// Load all textures
void Config::loadTextures(){
	if(!emptyPixel.loadFromFile(relPathToExePath("images/emptypixel.png"))){
		logSys.log("ERROR - config.cpp loadTextures()", "images/emptypixel.png loading failed!");
	} else {
		emptyPixel.setRepeated(true);
	}

	// HUD
	if(!spr_play_button.loadFromFile(relPathToExePath("images/play_button.png"))){
		logSys.log("ERROR - config.cpp loadTextures()", "images/play_button.png loading failed!");
	}
	if(!spr_pause_button.loadFromFile(relPathToExePath("images/pause_button.png"))){
		logSys.log("ERROR - config.cpp loadTextures()", "images/pause_button.png loading failed!");
	}
	if(!spr_stop_button.loadFromFile(relPathToExePath("images/stop_button.png"))){
		logSys.log("ERROR - config.cpp loadTextures()", "images/stop_button.png loading failed!");
	}
	if(!spr_next_button.loadFromFile(relPathToExePath("images/next_button.png"))){
		logSys.log("ERROR - config.cpp loadTextures()", "images/next_button.png loading failed!");
	}
	if(!spr_previous_button.loadFromFile(relPathToExePath("images/previous_button.png"))){
		logSys.log("ERROR - config.cpp loadTextures()", "images/previous_button.png loading failed!");
	}
}

// Load all shaders
void Config::loadShaders(){
	try{
		if(!shader_brightness.loadFromFile(relPathToExePath("shad.vert"), relPathToExePath("shad.frag"))){
			throw std::runtime_error("Brightness shader loading failed!");
		}
		if(!shader_glass.loadFromFile(relPathToExePath("shad_glass.frag"), sf::Shader::Fragment)){
			throw std::runtime_error("Glass shader loading failed!");
		}
	} catch(std::exception &e){
		logSys.log("ERROR - config.cpp loadShaders()", e.what());
	} catch(...){
		logSys.log("ERROR - config.cpp loadShaders()", "Unexpected error!");
	}
}

// Load all fonts
void Config::loadFonts(){
	try{
		if(!fBold.loadFromFile(relPathToExePath(s_fBold))){ 
			throw std::runtime_error("Primary font loading failed!");
		}
		if(!fRegular.loadFromFile(relPathToExePath(s_fRegular))){ 
			throw std::runtime_error("Secondary font loading failed!");
		}
		if(!fNormal.loadFromFile(relPathToExePath(s_fNormal))){ 
			throw std::runtime_error("Menu font loading failed!");
		}
	} catch(std::exception &e){
		logSys.log("ERROR - config.cpp loadFonts()", e.what());
	} catch(...){
		logSys.log("ERROR - config.cpp loadFonts()", "Unexpected error!");
	}
}

/*
	########################################
	Config constructor
*/

// Default constructor
Config::Config(){
	using json = nlohmann::json;
	json confg;

	// Load json file and save it to variable
	try{
		std::ifstream jsonRaw("config.json");
		if(jsonRaw.fail()){
			throw std::runtime_error("config.json loading failed!");
		}
		jsonRaw >> confg;
	} catch(std::exception &e){
		logSys.log("ERROR - config.cpp Config()", e.what());
	} catch(...){
		logSys.log("ERROR - config.cpp Config()", "Unexpected error!");
	}
	
	// Load all textures, settings etc.
	loadSettings(confg);
	loadTextures();
	loadShaders();
	loadFonts();
}

// Declare Config object
Logger logSys;
Config cfg;