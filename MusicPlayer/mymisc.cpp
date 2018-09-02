#include <iostream>
#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>
#include <boost/filesystem.hpp>
#include "bass.h"
#include "mymisc.h"
#include "config.h"

/*
	Button class
*/

//Set button text
void Button::setText(std::string str){
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

//Update button size
void Button::updateButton(){
	border.setSize(sf::Vector2f(width,height));
	border.setPosition(posX,posY);
}

//Draw Button on the screen
void Button::draw(sf::RenderWindow &win){
	win.draw(border);
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
Button::Button(std::string name, unsigned int x, unsigned int y, unsigned short wid, unsigned short hei, void(*funk)(std::vector<std::string>), std::vector<std::string> arguments, sf::Color back){
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

	width = textDisplayer.getGlobalBounds().width + wid*2;
	height = textDisplayer.getGlobalBounds().height + hei*2;
	textDisplayer.setPosition(x-textDisplayer.getGlobalBounds().left+wid,y+hei-textDisplayer.getGlobalBounds().top);   //Set position of text [1]

	updateButton();
}
Button::Button(std::string name, unsigned int x, unsigned int y, unsigned short wid, unsigned short hei, void(*funk)(std::vector<std::string>), std::vector<std::string> arguments){
	Button(name,x,y,wid,hei,funk,arguments,sf::Color(0,0,0,0));
}

//Return string with length of 2
std::string toDoubleChars(std::string x){
	if (x.size() == 1) {
		return "0" + x;
	} else {
		return x;
	}
}

//Print all devices that can play music
void printDevices() {
	BASS_DEVICEINFO info;
	for (int a = 1; BASS_GetDeviceInfo(a, &info); a++){
		std::cout << a << ".  " << info.name << "   |   " << info.driver << std::endl;
	}
}

//From double to string
std::string dblToString(double x){
	std::ostringstream strs;
	strs << x;
	return strs.str();
}

//From float to string
std::string floatToString(float x){
	std::ostringstream strs;
	strs << x;
	return strs.str();
}

//From int to string
std::string intToString(int x){
	std::ostringstream strs;
	strs << x;
	return strs.str();
}

//Change to human readable time from seconds
std::string toHumanTime(double ti){
	std::string str;
	int timeInt = floor(ti);
	int timeToCheck = timeInt / 3600;
	if (timeToCheck != 0){
		str += intToString(timeToCheck) + ":";
	}
	timeInt -= timeToCheck * 3600;
	timeToCheck = timeInt / 60;
	str += intToString(timeToCheck) + ":";
	timeInt -= timeToCheck * 60;
	str += toDoubleChars(intToString(timeInt));
	return str;
}
