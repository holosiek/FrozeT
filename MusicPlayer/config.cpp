#include <SFML/Graphics.hpp>
#include "bass.h"
#include "config.h"

/*
	Config class
*/

// This function is called on startup
void Config::startup(){
	if(!fBold.loadFromFile(s_fBold)){ 
		// Error callback
	}
	if(!fRegular.loadFromFile(s_fRegular)){ 
		// Error callback
	}
	if(!fNormal.loadFromFile(s_fNormal)){ 
		// Error callback
	}
	if(!shader_brightness.loadFromFile("shad.vert", "shad.frag")){
		// Error callback
	}
}

// Default constructor
Config::Config(){
	startup();
}

// Declare Config object
Config cfg;