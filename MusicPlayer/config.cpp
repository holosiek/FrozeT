// SFML
#include <SFML/Window.hpp>
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
	GetModuleFileNameA(nullptr, path, MAX_PATH);
	std::string::size_type pos = std::string(path).find_last_of("\\/");
	std::string pathDir = std::string(path).substr(0, pos)+"\\";
	return pathDir + a_relPath;
}

/*
	########################################
	Config setting functions
*/

// Set window color
void Config::setWindowColor(const int a_color){
	int color = a_color;
	short colorR = color/65025;
	color %= 65025;
	short colorG = color/255;
	color %= 255;
	short colorB = color;
	winBackground = sf::Color(colorR,colorG,colorB);
}
void Config::setWindowColor(const short a_r, const short a_g, const short a_b){
	winBackground = sf::Color(a_r, a_g, a_b);
}

// Set windows settings 
void Config::setWindowSettings(sf::Window& a_win){
	if(vsync){
		a_win.setVerticalSyncEnabled(true);
		a_win.setFramerateLimit(0);
	} else {
		a_win.setVerticalSyncEnabled(false);
		a_win.setFramerateLimit(winFPS);
	}
	a_win.setKeyRepeatEnabled(false);
}

/*
	########################################
	Config loading functions
*/

// Save to variables config options
void Config::loadSettings(nlohmann::json& a_json){
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
		smoothLevel     = a_json["smoothLevel"];

		// Window
		winWidth        = a_json["window_width"];
		winHeight       = a_json["window_height"];
		setWindowColor   (a_json["window_background_color"]);
		winFPS          = a_json["fps"];
		winTitle        = a_json["window_title"];
		isFullscreen    = a_json["window_fullscreen_mode"];
		drawHUD         = a_json["draw_hud"];
		vsync           = a_json["vsync"];

		// Fonts
		s_fBold         = a_json["primary_font"];
		s_fRegular      = a_json["secondary_font"];
		s_fNormal       = a_json["menu_font"];
	} catch(std::exception& e){
		Logger::log("ERROR - config.cpp loadSettings()", e.what());
	} catch(...){
		Logger::log("ERROR - config.cpp loadSettings()", "Unexpected error!");
	}
	Logger::log("INFO - config.cpp loadSettings()", "Settings loaded!");
}

// Load all textures
void Config::loadTextures(){
	if(!emptyPixel.loadFromFile(relPathToExePath("images/emptypixel.png"))){
		Logger::log("ERROR - config.cpp loadTextures()", "images/emptypixel.png loading failed!");
	} else {
		emptyPixel.setRepeated(true);
	}

	// HUD
	if(!spr_play_button.loadFromFile(relPathToExePath("images/play_button.png"))){
		Logger::log("ERROR - config.cpp loadTextures()", "images/play_button.png loading failed!");
	}
	if(!spr_pause_button.loadFromFile(relPathToExePath("images/pause_button.png"))){
		Logger::log("ERROR - config.cpp loadTextures()", "images/pause_button.png loading failed!");
	}
	if(!spr_stop_button.loadFromFile(relPathToExePath("images/stop_button.png"))){
		Logger::log("ERROR - config.cpp loadTextures()", "images/stop_button.png loading failed!");
	}
	if(!spr_next_button.loadFromFile(relPathToExePath("images/next_button.png"))){
		Logger::log("ERROR - config.cpp loadTextures()", "images/next_button.png loading failed!");
	}
	if(!spr_previous_button.loadFromFile(relPathToExePath("images/previous_button.png"))){
		Logger::log("ERROR - config.cpp loadTextures()", "images/previous_button.png loading failed!");
	}

	// Inform that textures are loaded
	Logger::log("INFO - config.cpp loadTextures()", "Textures loaded!");
}

// Load all shaders
void Config::loadShaders(){
	try{
		// Try to load shaders
		if(!shader_brightness.loadFromFile(relPathToExePath("shad.vert"), relPathToExePath("shad.frag"))){
			throw std::runtime_error("Brightness shader loading failed!");
		}
		if(!shader_glass.loadFromFile(relPathToExePath("shad_glass.frag"), sf::Shader::Fragment)){
			throw std::runtime_error("Glass shader loading failed!");
		}

		// Set shaders variables
		shader_brightness.setUniform("blur_radius", sf::Vector2f(0.003f, 0.003f));
		shader_glass.setUniform("blur_radius", sf::Vector2f(0.001f, 0.001f));
		Logger::log("INFO - config.cpp loadShaders()", "Updated shaders settings");

		// Inform that shaders are loaded
		Logger::log("INFO - config.cpp loadFonts()", "Shaders loaded!");
	} catch(std::exception& e){
		Logger::log("ERROR - config.cpp loadShaders()", e.what());
	} catch(...){
		Logger::log("ERROR - config.cpp loadShaders()", "Unexpected error!");
	}
}

// Load all fonts
void Config::loadFonts(){
	try{
		// Try to load fonts
		if(!fBold.loadFromFile(relPathToExePath(s_fBold))){ 
			throw std::runtime_error("Primary font loading failed!");
		}
		if(!fRegular.loadFromFile(relPathToExePath(s_fRegular))){ 
			throw std::runtime_error("Secondary font loading failed!");
		}
		if(!fNormal.loadFromFile(relPathToExePath(s_fNormal))){ 
			throw std::runtime_error("Menu font loading failed!");
		}

		// Inform that fonts are loaded
		Logger::log("INFO - config.cpp loadFonts()", "Fonts loaded!");
	} catch(std::exception& e){
		Logger::log("ERROR - config.cpp loadFonts()", e.what());
	} catch(...){
		Logger::log("ERROR - config.cpp loadFonts()", "Unexpected error!");
	}
}

/*
	########################################
	Config constructor
*/

// Default constructor
Config::Config(){
}

// Deconstructor
Config::~Config(){
}

// Init of Config
void Config::init(Config& a_cfg){
	using json = nlohmann::json;
	json confg;

	// Load json file and save it to variable
	try{
		std::ifstream jsonRaw("config.json");
		if(jsonRaw.fail()){
			throw std::runtime_error("config.json loading failed!");
		}
		jsonRaw >> confg;
	} catch(std::exception& e){
		Logger::log("ERROR - config.cpp Config()", e.what());
	} catch(...){
		Logger::log("ERROR - config.cpp Config()", "Unexpected error!");
	}
	
	// Load all textures, settings etc.
	a_cfg.loadSettings(confg);
	a_cfg.loadTextures();
	a_cfg.loadShaders();
	a_cfg.loadFonts();
}

// Declare Config object
Config cfg;