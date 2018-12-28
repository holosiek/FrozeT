#ifndef CONFIG
#define CONFIG

class Config {
	private:
		// Name of fonts
		const std::string s_fBold     = "Montserrat-Bold.otf";      //Bold Montserrat
		const std::string s_fRegular  = "Montserrat-Regular.otf";   //Regular Montserrat
		const std::string s_fNormal   = "PT-Sans-Normal.ttf";       //Normal PT Sans
	public:
		/* #########################################
			 ___      _              
			|   \ ___| |__ _  _ __ _ 
			| |) / -_) '_ \ || / _` |
			|___/\___|_.__/\_,_\__, |
							   |___/ 

		*/
		// If set true:
		// - Logs to console:
		// > which folder is selected in file browser dialog
		// > print how many songs is in folder when opened
		const bool debugMode            = true;
		// Prefix of all messages of debug mode
		std::wstring const debugPrefix  = L"[DEBUG] ";

		/* #########################################
			 ___      _   _   _              
			/ __| ___| |_| |_(_)_ _  __ _ ___
			\__ \/ -_)  _|  _| | ' \/ _` (_-<
			|___/\___|\__|\__|_|_||_\__, /__/
									|___/    

		*/
		// If true, window that is unfocused will has max 15 fps to save some GPU power
		const bool saveEnergy         = false;
		// If true, saves title of the song into the file inside working directory
		const bool saveTitleToFile    = true;


		/* #########################################
			  ___ _                       _ 
			 / __| |_  __ _ _ _  _ _  ___| |
			| (__| ' \/ _` | ' \| ' \/ -_) |
			 \___|_||_\__,_|_||_|_||_\___|_|
                                
		*/
		// DeviceID of device that will play music, -1 = default
		short deviceID                = 5;
		// Sampling frequency of channel
		const unsigned short freq     = 44100;
		// Freq of samples
		const DWORD fftfreq           = BASS_DATA_FFT4096;


		/* #########################################
			__      ___         _            
			\ \    / (_)_ _  __| |_____ __ __
			 \ \/\/ /| | ' \/ _` / _ \ V  V /
			  \_/\_/ |_|_||_\__,_\___/\_/\_/ 
                                 
		*/
		// Window width ad height
		unsigned short winWidth       = 960;
		unsigned short winHeight      = 480;
		// Window fps
		const unsigned short winFPS   = 60;
		// Window title
		const std::string winTitle    = "FrozeT";


		/* #########################################
			 __  __ _        
			|  \/  (_)___ __ 
			| |\/| | (_-</ _|
			|_|  |_|_/__/\__|
                 
		*/
		// Fonts
		sf::Font fBold, fRegular, fNormal;
		// Textures
		sf::Texture emptyPixel;
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

		void startup();
		Config();
};

extern Config cfg;

#endif CONFIG