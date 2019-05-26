// Standard
#include <iostream>
#include <string>
// SFML
#include <SFML/Graphics.hpp>
// Project Related
#include "button.hpp"
#include "config.hpp"

/*
	################################################
		SET functions
*/

void GUI::wButton::setIcon(sf::Texture &texture){
	icon.setTexture(texture);
}

void GUI::wButton::setFontSize(unsigned int size){
	labelContainer.setCharacterSize(size);
}

void GUI::wButton::setFont(sf::Font &font){
	labelContainer.setFont(font);
}

void GUI::wButton::setTextColor(sf::Color color){
	labelContainer.setFillColor(color);
}

void GUI::wButton::setSize(){
	size.x = padding.x*2 + labelContainer.getGlobalBounds().width;
	size.y = padding.y*2 + labelContainer.getGlobalBounds().height;
	backgroundRect.setSize(size);
}

void GUI::wButton::setValue(unsigned short arg_number){
	buttonValue = arg_number;
}

void GUI::wButton::setText(std::wstring str){
	label = str;
	labelContainer.setString(label);
}

void GUI::wButton::setPosition(GUI::Position arg_positionType, sf::Vector2f arg_position){
	// Set new values
	position = arg_position;
	positionType = arg_positionType;

	// Set new positions
	switch(positionType){
		case POS_NORMAL:
		default:
			backgroundRect.setPosition(position);
			icon.setPosition(position);
			labelContainer.setPosition(position.x+padding.x-labelContainer.getLocalBounds().left, position.y+padding.y-labelContainer.getLocalBounds().top);
			break;
		case POS_CENTER:
			backgroundRect.setPosition(sf::Vector2f(cfg.winWidth/2,cfg.winHeight/2)+position);
			icon.setPosition(sf::Vector2f(cfg.winWidth/2,cfg.winHeight/2)+position);
			labelContainer.setPosition(cfg.winWidth/2+position.x+padding.x-labelContainer.getLocalBounds().left, cfg.winHeight/2+position.y+padding.y-labelContainer.getLocalBounds().top);
			break;
		case POS_BOTTOM:
			backgroundRect.setPosition(sf::Vector2f(cfg.winWidth,cfg.winHeight)-position-size);
			icon.setPosition(sf::Vector2f(cfg.winWidth,cfg.winHeight)-position-size);
			labelContainer.setPosition(cfg.winWidth-position.x+padding.x-labelContainer.getLocalBounds().left-size.x, cfg.winHeight-position.y+padding.y-labelContainer.getLocalBounds().top-size.y);
			break;
	}
}

/*
	################################################
		UPDATE functions
*/

void GUI::wButton::update(){
	setPosition(positionType, position);
}

// Draw button on window
void GUI::wButton::draw(sf::RenderWindow &window){
	// Draw icon and background of the button
	window.draw(backgroundRect);
	window.draw(icon);
	// Draw text
	labelContainer.setString(label);
	window.draw(labelContainer);
}

/*
	################################################
		EVENT function
*/

bool GUI::wButton::isClicked(sf::Vector2i mousePos){
	switch(positionType){
		case POS_NORMAL:
		default:
			if(mousePos.x >= position.x && mousePos.x <= (position.x+size.x) && mousePos.y <= (position.y+size.y) && mousePos.y >= position.y){
				return true;
			}
			break;
		case POS_CENTER:
			if(mousePos.x >= cfg.winWidth/2+position.x && mousePos.x <= (cfg.winWidth/2+position.x+size.x) && mousePos.y <= (cfg.winHeight/2+position.y+size.y) && mousePos.y >= cfg.winHeight/2+position.y){
				return true;
			}
			break;
		case POS_BOTTOM:
			if(mousePos.x >= cfg.winWidth-position.x-size.x && mousePos.x <= (cfg.winWidth-position.x) && mousePos.y >= (cfg.winHeight-position.y-size.y) && mousePos.y <= cfg.winHeight-position.y){
				return true;
			}
			break;
	}
	return false;
}


/*
	################################################
		CONSTRUCTORS
*/


GUI::wButton::wButton(std::wstring arg_label, GUI::Position arg_positionType, sf::Vector2f arg_position, sf::Vector2f arg_padding, sf::Color arg_background) : label(arg_label), positionType(arg_positionType), position(arg_position), padding(arg_padding), background(arg_background){
	// Set background color of the button
	backgroundRect.setFillColor(arg_background);

	// Set default settings for button text
	setFont(cfg.fNormal);
	setFontSize(16);
	setTextColor(sf::Color::Black);
	setText(arg_label);

	// Set size of the button
	setSize();

	// Set position of the button
	setPosition(arg_positionType,arg_position);
};

/*
	################################################
		NORMAL BUTTON

*/

/*
	################################################
		SET functions
*/

void GUI::Button::setIcon(sf::Texture &texture){
	icon.setTexture(texture);
}

void GUI::Button::setFontSize(unsigned int size){
	labelContainer.setCharacterSize(size);
}

void GUI::Button::setFont(sf::Font &font){
	labelContainer.setFont(font);
}

void GUI::Button::setTextColor(sf::Color color){
	labelContainer.setFillColor(color);
}

void GUI::Button::setSize(){
	size.x = padding.x*2 + labelContainer.getGlobalBounds().width;
	size.y = padding.y*2 + labelContainer.getGlobalBounds().height;
	backgroundRect.setSize(size);
}

void GUI::Button::setValue(unsigned short arg_number){
	buttonValue = arg_number;
}

void GUI::Button::setText(std::string str){
	label = str;
	labelContainer.setString(label);
}

void GUI::Button::setPosition(GUI::Position arg_positionType, sf::Vector2f arg_position){
	// Set new values
	position = arg_position;
	positionType = arg_positionType;

	// Set new positions
	switch(positionType){
		case POS_NORMAL:
		default:
			backgroundRect.setPosition(position);
			icon.setPosition(position);
			labelContainer.setPosition(position.x+padding.x-labelContainer.getLocalBounds().left, position.y+padding.y-labelContainer.getLocalBounds().top);
			break;
		case POS_CENTER:
			backgroundRect.setPosition(sf::Vector2f(cfg.winWidth/2,cfg.winHeight/2)+position);
			icon.setPosition(sf::Vector2f(cfg.winWidth/2,cfg.winHeight/2)+position);
			labelContainer.setPosition(cfg.winWidth/2+position.x+padding.x-labelContainer.getLocalBounds().left, cfg.winHeight/2+position.y+padding.y-labelContainer.getLocalBounds().top);
			break;
		case POS_BOTTOM:
			backgroundRect.setPosition(sf::Vector2f(cfg.winWidth,cfg.winHeight)-position-size);
			icon.setPosition(sf::Vector2f(cfg.winWidth,cfg.winHeight)-position-size);
			labelContainer.setPosition(cfg.winWidth-position.x+padding.x-labelContainer.getLocalBounds().left-size.x, cfg.winHeight-position.y+padding.y-labelContainer.getLocalBounds().top-size.y);
			break;
	}
}

/*
	################################################
		UPDATE functions
*/

void GUI::Button::update(){
	setPosition(positionType, position);
}

// Draw button on window
void GUI::Button::draw(sf::RenderWindow &window){
	// Draw icon and background of the button
	window.draw(backgroundRect);
	window.draw(icon);
	// Draw text
	labelContainer.setString(label);
	window.draw(labelContainer);
}

/*
	################################################
		EVENT function
*/

bool GUI::Button::isClicked(sf::Vector2i mousePos){
	switch(positionType){
		case POS_NORMAL:
		default:
			if(mousePos.x >= position.x && mousePos.x <= (position.x+size.x) && mousePos.y <= (position.y+size.y) && mousePos.y >= position.y){
				return true;
			}
			break;
		case POS_CENTER:
			if(mousePos.x >= cfg.winWidth/2+position.x && mousePos.x <= (cfg.winWidth/2+position.x+size.x) && mousePos.y <= (cfg.winHeight/2+position.y+size.y) && mousePos.y >= cfg.winHeight/2+position.y){
				return true;
			}
			break;
		case POS_BOTTOM:
			if(mousePos.x >= cfg.winWidth-position.x-size.x && mousePos.x <= (cfg.winWidth-position.x) && mousePos.y >= (cfg.winHeight-position.y-size.y) && mousePos.y <= cfg.winHeight-position.y){
				return true;
			}
			break;
	}
	return false;
}


/*
	################################################
		CONSTRUCTORS
*/


GUI::Button::Button(std::string arg_label, GUI::Position arg_positionType, sf::Vector2f arg_position, sf::Vector2f arg_padding, sf::Color arg_background) : label(arg_label), positionType(arg_positionType), position(arg_position), padding(arg_padding), background(arg_background){
	// Set background color of the button
	backgroundRect.setFillColor(arg_background);

	// Set default settings for button text
	setFont(cfg.fNormal);
	setFontSize(16);
	setTextColor(sf::Color::Black);
	setText(arg_label);

	// Set size of the button
	setSize();

	// Set position of the button
	setPosition(arg_positionType,arg_position);
};

