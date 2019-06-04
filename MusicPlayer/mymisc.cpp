#include <iostream>
#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>
#include <boost/filesystem.hpp>
#include "bass.h"
#include "mymisc.hpp"
#include "config.hpp"

/*
	Button class

//Set button text
void Button::setText(std::wstring str){
	text = str;
	textDisplayer.setString(text);
}

//Set button font
void Button::setFont(sf::Font font){
	textDisplayer.setFont(font);
}

//Set button font size
void Button::setFontSize(unsigned int size){
	textDisplayer.setCharacterSize(size);
}

//Set icon of button
void Button::setIcon(sf::Texture &tex){
	icon.setTexture(tex);
}

//Update button size
void Button::updateButton(){
	border.setSize(sf::Vector2f(width,height));
	border.setPosition(posX,posY);
	icon.setPosition(posX,posY);
}

//Draw Button on the screen
void Button::draw(sf::RenderWindow &win){
	win.draw(border);
	win.draw(icon);
	textDisplayer.setString(text);
	win.draw(textDisplayer);
}

//Check if mouse is clicking on the button
int Button::checkIfClicked(sf::Vector2i mousePos){
	if(mousePos.x >= posX && mousePos.x <= (posX+width) && mousePos.y >= posY && mousePos.y <= (posY+height)){
		funcOnClick(funcArgs);
	} else {
		return 0;
	}
}

//Button constructors
Button::Button(std::wstring name, unsigned int x, unsigned int y, unsigned short wid, unsigned short hei, void(*funk)(std::vector<std::wstring>), std::vector<std::wstring> arguments, sf::Color back){
	posX = x;
	posY = y;
	background = back;
	funcOnClick = funk;
	funcArgs = arguments;
	border.setFillColor(back);

	// Default text properties
	text = name;
	textDisplayer.setFont(cfg.fNormal);
	textDisplayer.setCharacterSize(16);
	textDisplayer.setFillColor(sf::Color::Black);
	textDisplayer.setString(text);
	border.setTexture(&cfg.emptyPixel);

	width = textDisplayer.getGlobalBounds().width + wid*2;
	height = textDisplayer.getGlobalBounds().height + hei*2;
	textDisplayer.setPosition(x-textDisplayer.getGlobalBounds().left+wid,y+hei-textDisplayer.getGlobalBounds().top);   //Set position of text [1]

	updateButton();
}
Button::Button(std::wstring name, unsigned int x, unsigned int y, unsigned short wid, unsigned short hei, void(*funk)(std::vector<std::wstring>), std::vector<std::wstring> arguments){
	Button(name,x,y,wid,hei,funk,arguments,sf::Color(0,0,0,0));
}

*/
/*
	MISC
*/

//Print all devices that can play music
void printDevices() {
	BASS_DEVICEINFO info;
	for (int a = 1; BASS_GetDeviceInfo(a, &info); a++){
		std::cout << a << ".  " << info.name << "   |   " << info.driver << std::endl;
	}
}