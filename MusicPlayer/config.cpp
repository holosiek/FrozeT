#include <SFML/Graphics.hpp>
#include "bass.h"
#include "config.h"
#include <string>
#include <Windows.h>

/*
	Config class
*/

// This function is called on startup
void Config::startup(){
	// Take the executable path (Copied from StackOverflow, will take closer look later | https://stackoverflow.com/questions/875249/how-to-get-current-directory)
	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);
	std::string::size_type pos = std::string(path).find_last_of("\\/");
	std::string pathDir = std::string(path).substr(0, pos)+"\\";
	// Load Textures
	if(!fBold.loadFromFile(pathDir + s_fBold)){ 
		// Error callback
	}
	if(!fRegular.loadFromFile(pathDir + s_fRegular)){ 
		// Error callback
	}
	if(!fNormal.loadFromFile(pathDir + s_fNormal)){ 
		// Error callback
	}
	if(!shader_brightness.loadFromFile(pathDir + "shad.vert", pathDir + "shad.frag")){
		// Error callback
	}
	if(!shader_glass.loadFromFile(pathDir + "shad_glass.frag", sf::Shader::Fragment)){
		// Error callback
	}
	if(!emptyPixel.loadFromFile(pathDir + "images/emptypixel.png")){
		// Error callback
	} else {
		emptyPixel.setRepeated(true);
	}
}

// Default constructor
Config::Config(){
	startup();
}

// Declare Config object
Config cfg;