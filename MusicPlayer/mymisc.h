#ifndef MYMISC
#define MYMISC

class Button{
	private:
		unsigned int posX = 0;        //Position X on screen
		unsigned int posY = 0;        //Position Y on screen
		unsigned short width = 0;     //Width of button
		unsigned short height = 0;    //Height of button
		std::string text = "";        //Text written on the button
		sf::RectangleShape border;    //Rectangle behind button describing it borders
		sf::Color background;         //Background of button
		void (*funcOnClick)();        //Function to perform on click
	public:
		void updateButton();
		void draw(sf::RenderWindow &win);
		int checkIfClicked(sf::Vector2i mousePos);
		Button(unsigned int x, unsigned int y, unsigned short wid, unsigned short hei, void(*funk)(), sf::Color back);
		Button(unsigned int x, unsigned int y, unsigned short wid, unsigned short hei, void(*funk)());
};

void printDevices();
std::string dblToString(double);
std::string floatToString(float);
std::string intToString(int);
std::string toHumanTime(double);

#endif
