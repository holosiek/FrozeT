#include <SFML/Graphics.hpp>
#include "bass.h"
#include "config.h"
#include <string>
#include <Windows.h>

// Transform relative path to executable path [Used cuz of "default program" system]
std::string relPathToExePath(std::string relPath){
	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);
	std::string::size_type pos = std::string(path).find_last_of("\\/");
	std::string pathDir = std::string(path).substr(0, pos)+"\\";
	return pathDir + relPath;
}

/*  
	##################################################################################
*/

// Load Textures all textures
void Config::loadTextures(){
	if(!fBold.loadFromFile(relPathToExePath(s_fBold))){ 
		// Error callback
	}
	if(!fRegular.loadFromFile(relPathToExePath(s_fRegular))){ 
		// Error callback
	}
	if(!fNormal.loadFromFile(relPathToExePath(s_fNormal))){ 
		// Error callback
	}
	if(!shader_brightness.loadFromFile(relPathToExePath("shad.vert"), relPathToExePath("shad.frag"))){
		// Error callback
	}
	if(!shader_glass.loadFromFile(relPathToExePath("shad_glass.frag"), sf::Shader::Fragment)){
		// Error callback
	}
	if(!emptyPixel.loadFromFile(relPathToExePath("images/emptypixel.png"))){
		// Error callback
	} else {
		emptyPixel.setRepeated(true);
	}

	// HUD
	if(!spr_play_button.loadFromFile(relPathToExePath("images/play_button.png"))){
		// Error callback
	}
	if(!spr_pause_button.loadFromFile(relPathToExePath("images/pause_button.png"))){
		// Error callback
	}
	if(!spr_stop_button.loadFromFile(relPathToExePath("images/stop_button.png"))){
		// Error callback
	}
	if(!spr_next_button.loadFromFile(relPathToExePath("images/next_button.png"))){
		// Error callback
	}
	if(!spr_previous_button.loadFromFile(relPathToExePath("images/previous_button.png"))){
		// Error callback
	}
}

/*
	Config class
*/

// This function is called on startup
void Config::startup(){
	loadTextures();
}

// Default constructor
Config::Config(){
	startup();
}

// Declare Config object
Config cfg;