#ifndef MYMISC
#define MYMISC

class Button{
	private:
		// Button properties
		unsigned int posX = 0;                          //Position X on screen
		unsigned int posY = 0;                          //Position Y on screen
		unsigned short width = 0;                       //Width of button (Padding)
		unsigned short height = 0;                      //Height of button (Padding)
		// Text to display
		std::string text = "";                          //Text written on the button
		sf::Text textDisplayer;                         //Text object
		// Draw rectangle
		sf::RectangleShape border;                      //Rectangle behind button describing it borders
		sf::Color background;                           //Background of button
		// Function on click
		void (*funcOnClick)(std::vector<std::string>);  //Function to perform on click
		std::vector<std::string> funcArgs = {};         //Arguments of funcOnClick

	public:
		void updateButton();
		void draw(sf::RenderWindow &win);
		int checkIfClicked(sf::Vector2i mousePos);
		// Properties changers
		void setText(std::string str);
		void setFont(sf::Font font);
		void setFontSize(unsigned int size);
		// Constructors
		Button(std::string name, unsigned int x, unsigned int y, unsigned short wid, unsigned short hei, void(*funk)(std::vector<std::string>), std::vector<std::string> arguments, sf::Color back);
		Button(std::string name, unsigned int x, unsigned int y, unsigned short wid, unsigned short hei, void(*funk)(std::vector<std::string>), std::vector<std::string> arguments);
};

void printDevices();
std::string dblToString(double);
std::string floatToString(float);
std::string intToString(int);
std::string toHumanTime(double);

#endif
