// SFML
#include <SFML/Graphics.hpp>
// Standard
#include <iostream>
#include <string>
// Project Related
#include "button.hpp"
#include "config.hpp"

/*
	###############################################
		Abstract button class
	###############################################
*/
GUI::AButton::AButton(const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background) : position(a_position), padding(a_padding), backgroundColor(a_background), positionType(a_positionType){
}

GUI::AButton::~AButton(){
}

/*
	###############################################
		Button with text
	###############################################
*/

// Check if

void GUI::Button::checkIfHover(const sf::Vector2i& a_mousePos){
	switch(positionType){
		case POS_NORMAL:
		default:
			if(a_mousePos.x >= position.x && a_mousePos.x <= (position.x + size.x) && a_mousePos.y <= (position.y + size.y) && a_mousePos.y >= position.y) {
				backgroundShape.setFillColor(backgroundHover);
			} else {
				backgroundShape.setFillColor(backgroundColor);
			}
			break;
		case POS_CENTER:
			if(a_mousePos.x >= cfg.winWidth / 2 + position.x && a_mousePos.x <= (cfg.winWidth / 2 + position.x + size.x) && a_mousePos.y <= (cfg.winHeight / 2 + position.y + size.y) && a_mousePos.y >= cfg.winHeight / 2 + position.y) {
				backgroundShape.setFillColor(backgroundHover);
			} else {
				backgroundShape.setFillColor(backgroundColor);
			}
			break;
		case POS_BOTTOM:
			if(a_mousePos.x >= cfg.winWidth - position.x - size.x && a_mousePos.x <= (cfg.winWidth - position.x) && a_mousePos.y >= (cfg.winHeight - position.y - size.y) && a_mousePos.y <= cfg.winHeight - position.y) {
				backgroundShape.setFillColor(backgroundHover);
			} else {
				backgroundShape.setFillColor(backgroundColor);
			}
			break;
	}
}

bool GUI::Button::checkIfClicked(const sf::Vector2i& a_mousePos){
	switch (positionType) {
		case POS_NORMAL:
		default:
			if(a_mousePos.x >= position.x && a_mousePos.x <= (position.x + size.x) && a_mousePos.y <= (position.y + size.y) && a_mousePos.y >= position.y) {
				return true;
			}
			break;
		case POS_CENTER:
			if(a_mousePos.x >= cfg.winWidth / 2 + position.x && a_mousePos.x <= (cfg.winWidth / 2 + position.x + size.x) && a_mousePos.y <= (cfg.winHeight / 2 + position.y + size.y) && a_mousePos.y >= cfg.winHeight / 2 + position.y) {
				return true;
			}
			break;
		case POS_BOTTOM:
			if(a_mousePos.x >= cfg.winWidth - position.x - size.x && a_mousePos.x <= (cfg.winWidth - position.x) && a_mousePos.y >= (cfg.winHeight - position.y - size.y) && a_mousePos.y <= cfg.winHeight - position.y) {
				return true;
			}
			break;
	}
	return false;
}

// Setters

void GUI::Button::setSize(){
	size.x = padding.x*2 + labelShape.getGlobalBounds().width;
	size.y = padding.y*2 + labelShape.getGlobalBounds().height;
	backgroundShape.setSize(size);
}

void GUI::Button::setFontSize(const unsigned int& a_size){
	labelShape.setCharacterSize(a_size);
}

void GUI::Button::setFont(sf::Font& a_font){
	labelShape.setFont(a_font);
}

void GUI::Button::setTextColor(const sf::Color& a_color){
	labelShape.setFillColor(a_color);
}

void GUI::Button::setText(const std::string& a_label){
	labelShape.setString(a_label);
}

void GUI::Button::setText(const std::wstring& a_label){
	labelShape.setString(a_label);
}

void GUI::Button::setPosition(const GUI::Position& a_positionType, const sf::Vector2f& a_position){
	// Set new values
	position = a_position;
	positionType = a_positionType;

	// Set new positions
	switch(positionType){
		case POS_NORMAL:
		default:
			backgroundShape.setPosition(position);
			labelShape.setPosition(position.x+padding.x-labelShape.getLocalBounds().left, position.y+padding.y-labelShape.getLocalBounds().top);
			break;
		case POS_CENTER:
			backgroundShape.setPosition(sf::Vector2f(cfg.winWidth/2,cfg.winHeight/2)+position);
			labelShape.setPosition(cfg.winWidth/2+position.x+padding.x-labelShape.getLocalBounds().left, cfg.winHeight/2+position.y+padding.y-labelShape.getLocalBounds().top);
			break;
		case POS_BOTTOM:
			backgroundShape.setPosition(sf::Vector2f(cfg.winWidth,cfg.winHeight)-position-size);
			labelShape.setPosition(cfg.winWidth-position.x+padding.x-labelShape.getLocalBounds().left-size.x, cfg.winHeight-position.y+padding.y-labelShape.getLocalBounds().top-size.y);
			break;
	}
}

// Updaters
void GUI::Button::update(){
	setSize();
	setPosition(positionType, position);
}

// Constructor, Deconstructor and draw
GUI::Button::Button(const std::string& a_label, const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background) : Button(a_positionType, a_position, a_padding, a_background){
	setText(a_label);
}

GUI::Button::Button(const std::wstring& a_label, const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background) : Button(a_positionType, a_position, a_padding, a_background){
	setText(a_label);
}

GUI::Button::Button(const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background) : AButton(a_positionType, a_position, a_padding, a_background){
	// Set background color
	backgroundShape.setFillColor(a_background);

	// Set default settings for button text
	setFont(cfg.fNormal);
	setFontSize(16);
	setTextColor(sf::Color::Black);

	// Set background hover color
	backgroundHover.r = (backgroundColor.r < 123) ? (backgroundColor.r + 20) : (backgroundColor.r - 20);
	backgroundHover.g = (backgroundColor.g < 123) ? (backgroundColor.g + 20) : (backgroundColor.g - 20);
	backgroundHover.b = (backgroundColor.b < 123) ? (backgroundColor.b + 20) : (backgroundColor.b - 20);
}

GUI::Button::~Button(){
}

void GUI::Button::draw(sf::RenderWindow& a_win){
	a_win.draw(backgroundShape);
	a_win.draw(labelShape);
}

/*
	###############################################
		Checkbox with text
	###############################################
*/

// Check if

void GUI::Checkbox::checkIfHover(const sf::Vector2i& a_mousePos){
	if(!isSelected){
		switch(positionType){
			case POS_NORMAL:
			default:
				if(a_mousePos.x >= position.x && a_mousePos.x <= (position.x + size.x) && a_mousePos.y <= (position.y + size.y) && a_mousePos.y >= position.y) {
					backgroundShape.setFillColor(backgroundHover);
				} else {
					backgroundShape.setFillColor(backgroundColor);
				}
				break;
			case POS_CENTER:
				if(a_mousePos.x >= cfg.winWidth / 2 + position.x && a_mousePos.x <= (cfg.winWidth / 2 + position.x + size.x) && a_mousePos.y <= (cfg.winHeight / 2 + position.y + size.y) && a_mousePos.y >= cfg.winHeight / 2 + position.y) {
					backgroundShape.setFillColor(backgroundHover);
				} else {
					backgroundShape.setFillColor(backgroundColor);
				}
				break;
			case POS_BOTTOM:
				if(a_mousePos.x >= cfg.winWidth - position.x - size.x && a_mousePos.x <= (cfg.winWidth - position.x) && a_mousePos.y >= (cfg.winHeight - position.y - size.y) && a_mousePos.y <= cfg.winHeight - position.y) {
					backgroundShape.setFillColor(backgroundHover);
				} else {
					backgroundShape.setFillColor(backgroundColor);
				}
				break;
		}
	}
}

bool GUI::Checkbox::checkIfClicked(const sf::Vector2i& a_mousePos){
	switch (positionType) {
		case POS_NORMAL:
		default:
			if(a_mousePos.x >= position.x && a_mousePos.x <= (position.x + size.x) && a_mousePos.y <= (position.y + size.y) && a_mousePos.y >= position.y) {
				setSelected();
				return true;
			}
			break;
		case POS_CENTER:
			if(a_mousePos.x >= cfg.winWidth / 2 + position.x && a_mousePos.x <= (cfg.winWidth / 2 + position.x + size.x) && a_mousePos.y <= (cfg.winHeight / 2 + position.y + size.y) && a_mousePos.y >= cfg.winHeight / 2 + position.y) {
				setSelected();
				return true;
			}
			break;
		case POS_BOTTOM:
			if(a_mousePos.x >= cfg.winWidth - position.x - size.x && a_mousePos.x <= (cfg.winWidth - position.x) && a_mousePos.y >= (cfg.winHeight - position.y - size.y) && a_mousePos.y <= cfg.winHeight - position.y) {
				setSelected();
				return true;
			}
			break;
	}
	return false;
}

// Setters

void GUI::Checkbox::setSize(){
	if(sizingType == GUI::Sizing::SZ_AUTO){
		size.x = padding.x*2 + labelShape.getGlobalBounds().width;
		size.y = padding.y*2 + labelShape.getGlobalBounds().height;
	}
	backgroundShape.setSize(size);
}

void GUI::Checkbox::setSize(const sf::Vector2f& a_size){
	sizingType = GUI::Sizing::SZ_CUSTOM;
	size.x = a_size.x;
	size.y = a_size.y;
	backgroundShape.setSize(size);
}

void GUI::Checkbox::setFontSize(const unsigned int& a_size){
	labelShape.setCharacterSize(a_size);
}

void GUI::Checkbox::setFont(sf::Font& a_font){
	labelShape.setFont(a_font);
}

void GUI::Checkbox::setTextColor(const sf::Color& a_color){
	labelShape.setFillColor(a_color);
}

void GUI::Checkbox::setText(const std::string& a_label){
	labelShape.setString(a_label);
}

void GUI::Checkbox::setText(const std::wstring& a_label){
	labelShape.setString(a_label);
}

void GUI::Checkbox::setText(const char* a_label){
	labelShape.setString(a_label);
}

void GUI::Checkbox::setPosition(const GUI::Position& a_positionType, const sf::Vector2f& a_position){
	// Set new values
	position = a_position;
	positionType = a_positionType;

	// Set new positions
	switch(positionType){
		case POS_NORMAL:
		default:
			backgroundShape.setPosition(position);
			labelShape.setPosition(position.x+padding.x-labelShape.getLocalBounds().left, position.y+padding.y-labelShape.getLocalBounds().top);
			break;
		case POS_CENTER:
			backgroundShape.setPosition(sf::Vector2f(cfg.winWidth/2,cfg.winHeight/2)+position);
			labelShape.setPosition(cfg.winWidth/2+position.x+padding.x-labelShape.getLocalBounds().left, cfg.winHeight/2+position.y+padding.y-labelShape.getLocalBounds().top);
			break;
		case POS_BOTTOM:
			backgroundShape.setPosition(sf::Vector2f(cfg.winWidth,cfg.winHeight)-position-size);
			labelShape.setPosition(cfg.winWidth-position.x+padding.x-labelShape.getLocalBounds().left-size.x, cfg.winHeight-position.y+padding.y-labelShape.getLocalBounds().top-size.y);
			break;
	}
}

void GUI::Checkbox::setSelected(){
	backgroundShape.setFillColor(backgroundSelected);
	isSelected = true;
}

// Updaters
void GUI::Checkbox::update(){
	setSize();
	setPosition(positionType, position);
}

void GUI::Checkbox::updateSelected(){
	backgroundShape.setFillColor(backgroundColor);
	isSelected = false;
}

// Constructor, Deconstructor and draw
GUI::Checkbox::Checkbox(const std::string& a_label, const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background) : Checkbox(a_positionType, a_position, a_padding, a_background){
	setText(a_label);
}

GUI::Checkbox::Checkbox(const std::wstring& a_label, const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background) : Checkbox(a_positionType, a_position, a_padding, a_background){
	setText(a_label);
}

GUI::Checkbox::Checkbox(const char* a_label, const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background) : Checkbox(a_positionType, a_position, a_padding, a_background){
	setText(a_label);
}

GUI::Checkbox::Checkbox(const Position& a_positionType, const sf::Vector2f& a_position, const sf::Vector2f& a_padding, const sf::Color& a_background) : AButton(a_positionType, a_position, a_padding, a_background){
	// Set background color
	backgroundShape.setFillColor(a_background);

	// Set default settings for Checkbox text
	setFont(cfg.fNormal);
	setFontSize(14);
	setTextColor(sf::Color::Black);

	// Set background hover color
	backgroundHover.r = (backgroundColor.r < 123) ? (backgroundColor.r + 20) : (backgroundColor.r - 20);
	backgroundHover.g = (backgroundColor.g < 123) ? (backgroundColor.g + 20) : (backgroundColor.g - 20);
	backgroundHover.b = (backgroundColor.b < 123) ? (backgroundColor.b + 20) : (backgroundColor.b - 20);

	// Set background selected color
	backgroundSelected.r = (backgroundColor.r < 123) ? (backgroundColor.r + 50) : (backgroundColor.r - 50);
	backgroundSelected.g = (backgroundColor.g < 123) ? (backgroundColor.g + 50) : (backgroundColor.g - 50);
	backgroundSelected.b = (backgroundColor.b < 123) ? (backgroundColor.b + 50) : (backgroundColor.b - 50);
}

GUI::Checkbox::~Checkbox(){}

// Cringe [move&copy] Semantics
GUI::Checkbox::Checkbox(const Checkbox& a_old) noexcept : AButton(a_old.positionType, a_old.position, a_old.padding, a_old.backgroundColor){
	labelShape.setString(a_old.labelShape.getString());
	backgroundHover = a_old.backgroundHover;
}

GUI::Checkbox::Checkbox(Checkbox&& a_old) noexcept : GUI::Checkbox(std::move(a_old.positionType), std::move(a_old.position), std::move(a_old.padding), std::move(a_old.backgroundColor)){
	labelShape.setString(a_old.labelShape.getString());
}


GUI::Checkbox& GUI::Checkbox::operator=(const GUI::Checkbox& a_old) noexcept{
	positionType = a_old.positionType;
	position = a_old.position;
	padding = a_old.padding;
	backgroundColor = a_old.backgroundColor;
	labelShape.setString(a_old.labelShape.getString());
	backgroundHover = a_old.backgroundHover;
	return *this;
}

GUI::Checkbox& GUI::Checkbox::operator=(GUI::Checkbox&& a_old) noexcept{
	if(&a_old == this)
		return *this;
	positionType = a_old.positionType;
	position = a_old.position;
	padding = a_old.padding;
	backgroundColor = a_old.backgroundColor;
	labelShape.setString(a_old.labelShape.getString());
	backgroundHover = a_old.backgroundHover;
	return *this;
}

void GUI::Checkbox::draw(sf::RenderWindow& a_win){
	a_win.draw(backgroundShape);
	a_win.draw(labelShape);
}