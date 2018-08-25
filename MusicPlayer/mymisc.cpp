#include <iostream>
#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>
#include <boost/filesystem.hpp>
#include "bass.h"
#include "mymisc.h"

void Button::updateButton(){
	border.setSize(sf::Vector2f(width,height));
	border.setPosition(posX,posY);
}

Button::Button(unsigned int x,unsigned int y,unsigned short wid,unsigned short hei){
	posX = x;
	posY = y;
	width = wid;
	height = hei;
	border.setFillColor(sf::Color::Yellow);
}

//return string with length of 2
std::string dd(std::string x) {
	if (x.size() == 1) {
		return "0" + x;
	}
	else {
		return x;
	}
}

//Print all devices that can play music
void printDevices() {
	BASS_DEVICEINFO info;
	for (int a = 1; BASS_GetDeviceInfo(a, &info); a++) {
		std::cout << a << ".  " << info.name << "   |   " << info.driver << std::endl;
	}
}

//From double to string
std::string dblToString(double x) {
	std::ostringstream strs;
	strs << x;
	return strs.str();
}

//From float to string
std::string floatToString(float x) {
	std::ostringstream strs;
	strs << x;
	return strs.str();
}

//From int to string
std::string intToString(int x) {
	std::ostringstream strs;
	strs << x;
	return strs.str();
}

//Change to human readable time from seconds
std::string toHumanTime(double ti) {
	std::string str;
	int timeInt = floor(ti);
	int timeToCheck = timeInt / 3600;
	if (timeToCheck != 0) {
		str += intToString(timeToCheck) + ":";
	}
	timeInt -= timeToCheck * 3600;
	timeToCheck = timeInt / 60;
	str += intToString(timeToCheck) + ":";
	timeInt -= timeToCheck * 60;
	str += dd(intToString(timeInt));
	return str;
}
