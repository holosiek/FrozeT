#pragma once

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
	enum Position{
		POS_NORMAL,   // Start positioning from (0,0)
		POS_CENTER,   // Start positioning from center of the window
		POS_BOTTOM    // Start positioning from the bottom right of the window
	};
	enum Sizing{
		SZ_AUTO,      // Automatically size button by adding padding and checking char size
		SZ_CUSTOM,    // Custom size
	};

	/*
		###############################################
			Abstract class of the button
		###############################################
	*/

	class AButton{
		protected:
			// General stuff
			Position positionType;                      // From where button should position
			Sizing sizingType = SZ_AUTO;                // Sizing type
			sf::Vector2f position;                      // Position of the button
			sf::Vector2f padding;                       // Padding of the button
			sf::Vector2f size;                          // Size of clickable region

			// Background
			sf::RectangleShape backgroundShape;         // Rectangle behind the button, which describes it's borders
			sf::Color backgroundColor;                  // Background of button
			sf::Color backgroundHover;                  // Background on hover

		public:
			AButton(const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background);
			virtual ~AButton();

			virtual void draw(sf::RenderWindow& a_win){};
	};

	/*
		###############################################
			Text button
		###############################################
	*/

	class Button final : public AButton{
		private:
			sf::Text labelShape;
		public:
			// Check if
			void checkIfHover(const sf::Vector2i& a_mousePos);
			bool checkIfClicked(const sf::Vector2i& a_mousePos);

			// Setters
			void setSize();
			void setPosition(const Position& a_positionType, const sf::Vector2f& a_position);
			void setText(const std::string& a_label);
			void setText(const std::wstring& a_label);
			void setTextColor(const sf::Color& a_color);
			void setFontSize(const unsigned int& a_size);
			void setFont(sf::Font& a_font);

			// Updaters
			void update();
			
			// Constructor, Deconstructor and draw
			Button(const std::string& a_label, const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background);
			Button(const std::wstring& a_label, const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background);
			Button(const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background);
			virtual ~Button();
			virtual void draw(sf::RenderWindow& a_win);
	};

	/*
		###############################################
			Checkbox button
		###############################################
	*/

	class Checkbox final : public AButton{
		private:
			sf::Text labelShape;
			sf::Color backgroundSelected;
			bool isSelected = false;
		public:
			// Check if
			void checkIfHover(const sf::Vector2i& a_mousePos);
			bool checkIfClicked(const sf::Vector2i& a_mousePos);

			// Setters
			void setSize();
			void setSize(const sf::Vector2f& a_size);
			void setPosition(const Position& a_positionType, const sf::Vector2f& a_position);
			void setText(const std::string& a_label);
			void setText(const std::wstring& a_label);
			void setText(const char* a_label);
			void setTextColor(const sf::Color& a_color);
			void setFontSize(const unsigned int& a_size);
			void setFont(sf::Font& a_font);
			void setSelected();

			// Updaters
			void update();
			void updateSelected();

			// Constructor, Deconstructor and draw
			Checkbox(const std::string& a_label, const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background);
			Checkbox(const std::wstring& a_label, const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background);
			Checkbox(const char* a_label, const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background);
			Checkbox(const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background);
			Checkbox(const Checkbox& a_old) noexcept;
			Checkbox(Checkbox&& a_old) noexcept;
			GUI::Checkbox& operator=(const GUI::Checkbox& a_old) noexcept;
			GUI::Checkbox& operator=(GUI::Checkbox&& a_old) noexcept;
			virtual ~Checkbox();
			virtual void draw(sf::RenderWindow& a_win);
	};

}