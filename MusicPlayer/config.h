#ifndef CONFIG
#define CONFIG

class Config {
	private:
		// Name of fonts
		const std::string s_fBold     = "Montserrat-Bold.otf";      //Bold
		const std::string s_fNormal   = "Montserrat-Regular.otf";   //Regular
	public:
		// If true, window that is unfocused will has max 15 fps to save some GPU power
		const bool saveEnergy         = false;
		// If true, saves title of the song into the file inside working directory
		const bool saveTitleToFile    = true;

		// Fonts
		sf::Font fBold, fNormal;
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
};

#endif