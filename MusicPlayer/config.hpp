#pragma once

// SFML
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
// Bass
#include "bass.h"
// JSON
#include <nlohmann/json.hpp>

class Config{
	private:
		/*
			#####################################
			Name of fonts
		*/

		std::string s_fBold     = "Montserrat-Bold.otf"; //Bold Montserrat 
		std::string s_fRegular  = "Montserrat-Regular.otf";   //Regular Montserrat 
		std::string s_fNormal   = "PT-Sans-Normal.ttf";       //Normal PT Sans

		/*
			#####################################
			Functions
		*/
		
		void loadTextures();
		void loadShaders();
		void loadFonts();
		void loadSettings(nlohmann::json &a_json);
	public:
		/*
			#####################################
			Main and debug variables
		*/

		// What's the version of the program?
		std::string programVersion = "";

		// Should debug messages be logged?
		bool debugMode = false;

		/*
			#####################################
			Settings
		*/

		// If true, window that is unfocused will have max 15 fps to save some GPU power
		bool saveEnergy = false;

		// If true, saves title of the song into the file inside working directory
		bool saveTitleToFile = true;


		/*
			#####################################
			Channel
		*/

		// Channel storaging var
		HSTREAM channel = NULL;

		// DeviceID of device that will play music, -1 = default
		short deviceID = 5;

		// Sampling frequency of channel
		unsigned short freq  = 44100;

		// Freq of samples
		DWORD fftfreq = BASS_DATA_FFT4096;

		// Smoothness level
		unsigned short smoothLevel = 1;


		/*
			#####################################
			Window
		*/

		// Window width ad height
		float winWidth  = 960.0f;
		float winHeight = 480.0f;

		// Window fps
		unsigned short winFPS = 144;

		// Window title
		std::string winTitle = "FrozeT";

		// Is window fullscreen?
		bool isFullscreen = false;

		// Is HUD on?
		bool drawHUD = true;

		// Should window vsync?
		bool vsync = false;

		// Is window clicked
		bool isClickedWindow = false;

		// What is the color of the window?
		sf::Color winBackground = sf::Color(123,123,123);


		/*
			#####################################
			Misc.
		*/

		// Fonts
		sf::Font fBold, fRegular, fNormal;
		
		// Textures
		sf::Texture spr_blankAlbum, emptyPixel, spr_play_button, spr_next_button, spr_previous_button, spr_pause_button, spr_stop_button;
		
		// Shaders
		sf::Shader shader_brightness, shader_glass;

		// Red/Green/Blue colors
		const sf::Color red           = sf::Color(255,0,0,255);
		const sf::Color green         = sf::Color(0,255,0,255);
		const sf::Color blue          = sf::Color(0,0,255,255);

		// Shades of grey
		const sf::Color black         = sf::Color(0,0,0,255);
		const sf::Color darker_grey   = sf::Color(31,31,31,255);
		const sf::Color dark_grey     = sf::Color(62,62,62,255);
		const sf::Color grey          = sf::Color(123,123,123,255);
		const sf::Color light_grey    = sf::Color(185,185,185,255);
		const sf::Color lighter_grey  = sf::Color(216,216,216,255);
		const sf::Color white         = sf::Color(255,255,255,255);

		void setWindowColor(const int a_color);
		void setWindowColor(const short a_r, const short a_g, const short a_b);
		void setWindowSettings(sf::Window& a_win);
		void static init(Config& a_cfg);

		/*
			#####################################
			Constructors
		*/

		Config();
		~Config();
};

extern Config cfg;