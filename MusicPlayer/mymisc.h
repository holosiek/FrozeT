#ifndef MYMISC
#define MYMISC

class Button{
	private:
		unsigned int posX = 0;
		unsigned int posY = 0;
		unsigned short width = 0;
		unsigned short height = 0;
		sf::Color background;
	public:
		sf::RectangleShape border;
		void updateButton();
		void draw(sf::RenderWindow &win);
		Button(unsigned int x, unsigned int y, unsigned short wid, unsigned short hei, sf::Color back);
		Button(unsigned int x, unsigned int y, unsigned short wid, unsigned short hei);
};

void printDevices();
std::string dblToString(double);
std::string floatToString(float);
std::string intToString(int);
std::string toHumanTime(double);

#endif
