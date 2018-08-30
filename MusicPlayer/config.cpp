#include <SFML/Graphics.hpp>
#include "config.h"

/*
	Config class
*/

//This function is called on startup
void Config::startup(){
	if (!fBold.loadFromFile(s_fBold)){ 
		//Error callback
	}
	if (!fNormal.loadFromFile(s_fNormal)){ 
		//Error callback
	}
}