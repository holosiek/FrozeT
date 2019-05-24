#ifndef BUTTON
#define BUTTON

/*
	################################################################
	>>> Button Class

	DESCRIPTION:
	Creates a button, which can perform various functions passed in constructor

	CONSTRUCTORS:
	Button(std::wstring arg_label, sf::Vector2u arg_position, sf::Vector2u arg_padding, sf::Color arg_background)
	
	std::wstring label:
		- displayed text on button
	sf::Vector2u position:
		- X & Y position in window
	sf::Vector2u padding:
		- top/bottom & left/right padding of the button
	sf::Color background:
		- background to display on the back
*/
namespace GUI {
	enum Position {
		POS_NORMAL,   // Start positioning from (0,0)
		POS_CENTER,   // Start positioning from center of the window
		POS_BOTTOM    // Start positioning from the bottom right of the window
	};
	class wButton {
		private:
			// General stuff
			Position positionType;                      // From where button should position
			sf::Vector2f position;                      // Position of the button
			sf::Vector2f padding;                       // Padding of the button
			sf::Vector2f size;                          // Size of clickable region

			// Button label
			std::wstring label;                         // Displayed text on the button
			sf::Text labelContainer;

			// Background
			sf::RectangleShape backgroundRect;          // Rectangle behind the button, which describes it's borders
			sf::Color background;                       // Background of button

			// Icon
			sf::Sprite icon;

			// Argument
			unsigned short buttonValue = 0;
		public:
			void setIcon(sf::Texture &texture);
			void setFontSize(unsigned int size);
			void setTextColor(sf::Color color);
			void setFont(sf::Font &font);
			void setText(std::wstring str);
			void setSize();
			void setValue(unsigned short arg_number);
			void setPosition(Position arg_positionType, sf::Vector2f arg_position);

			bool isClicked(sf::Vector2i mousePos);

			void update();
			void draw(sf::RenderWindow &win);

			wButton(std::wstring arg_label, Position arg_positionType, sf::Vector2f arg_position, sf::Vector2f arg_padding, sf::Color arg_background);
	};
	class Button {
		private:
			// General stuff
			Position positionType;                      // From where button should position
			sf::Vector2f position;                      // Position of the button
			sf::Vector2f padding;                       // Padding of the button
			sf::Vector2f size;                          // Size of clickable region

			// Button label
			std::string label;                         // Displayed text on the button
			sf::Text labelContainer;

			// Background
			sf::RectangleShape backgroundRect;          // Rectangle behind the button, which describes it's borders
			sf::Color background;                       // Background of button

			// Icon
			sf::Sprite icon;

			// Argument
		public:
			unsigned short buttonValue = 0;

			void setIcon(sf::Texture &texture);
			void setFontSize(unsigned int size);
			void setTextColor(sf::Color color);
			void setFont(sf::Font &font);
			void setText(std::string str);
			void setSize();
			void setValue(unsigned short arg_number);
			void setPosition(Position arg_positionType, sf::Vector2f arg_position);

			bool isClicked(sf::Vector2i mousePos);

			void update();
			void draw(sf::RenderWindow &win);

			Button(std::string arg_label, Position arg_positionType, sf::Vector2f arg_position, sf::Vector2f arg_padding, sf::Color arg_background);
	};
}

#endif BUTTON