#ifndef CONFIG
#define CONFIG

class Config {
	public:
		// Red/Green/Blue colors
		const sf::Color red           = sf::Color(255,0,0,1);
		const sf::Color green         = sf::Color(0,255,0,1);
		const sf::Color blue          = sf::Color(0,0,255,1);
		// Shades of grey
		const sf::Color black         = sf::Color(0,0,0,1);
		const sf::Color dark_grey     = sf::Color(185,185,185,1);
		const sf::Color grey          = sf::Color(123,123,123,1);
		const sf::Color light_grey    = sf::Color(62,62,62,1);
		const sf::Color white         = sf::Color(255,255,255,1);
};

#endif