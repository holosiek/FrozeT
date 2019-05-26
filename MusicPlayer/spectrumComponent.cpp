#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "bass.h"
#include "spectrumComponent.hpp"
#include "config.hpp"
#include "mymisc.hpp"

ProgressBar::ProgressBar(){
	// Change "duration and current time" text style
	progressBarTime.setFont(cfg.fBold);
	progressBarTime.setCharacterSize(18);
	progressBarTime.setFillColor(sf::Color::White);
	
	// Change "progress bar background" style
	progressBarBack.setSize(sf::Vector2f(cfg.winWidth - 20, 6));
	progressBarBack.setPosition(10, cfg.winHeight - 16);
	progressBarBack.setFillColor(sf::Color(40, 40, 40, 255));

	// Change "progress bar front" style
	progressBarFront.setSize(sf::Vector2f(cfg.winWidth - 20, 6));
	progressBarFront.setPosition(10, cfg.winHeight - 16);
	progressBarFront.setFillColor(sf::Color::White);
}

void ProgressBar::setLength(sf::Vector2f amount){
	progressBarFront.setSize(amount);
}

void ProgressBar::setText(std::string text){
	progressBarTime.setString(text);
	progressBarTime.setPosition(cfg.winWidth-10-progressBarTime.getLocalBounds().width, cfg.winHeight-42);
}

void ProgressBar::setTimeAndDuration(double tmp_duration){
	duration = tmp_duration;
}

void ProgressBar::resize(){
	progressBarBack.setSize(sf::Vector2f(cfg.winWidth - 20, 6));
	progressBarBack.setPosition(10, cfg.winHeight - 16);
	progressBarFront.setSize(sf::Vector2f(cfg.winWidth - 20, 6));
	progressBarFront.setPosition(10, cfg.winHeight - 16);
}

void ProgressBar::draw(sf::RenderWindow &windowToDrawOn){
	windowToDrawOn.draw(progressBarBack);
	windowToDrawOn.draw(progressBarFront);
	windowToDrawOn.draw(progressBarTime);
}

double ProgressBar::onClick(sf::Vector2i cords){
	if(cords.x >= 10 && cords.x <= cfg.winWidth-10 && cords.y >= cfg.winHeight - 16 && cords.y <= cfg.winHeight - 10){
		return ((double)(cords.x-10)/(cfg.winWidth-20))*duration;
	}
	return -1;
}

SpectrumComp::SpectrumComp(){
	// ######################################################### CHANGE STYLES

	// Change "album cover image" style
	albumCover.setSize(sf::Vector2f(128,128));
	albumCover.setPosition(cfg.winWidth*0.15,cfg.winHeight*0.65);
	albumCover.setTexture(&texture);
	albumCover.setTextureRect(sf::IntRect(0,0,albumCover.getTexture()->getSize().x,albumCover.getTexture()->getSize().y));

	// Change "artist text" and it's shadow style
	authorText.setFont(cfg.fBold);
	authorText.setCharacterSize(50);
	authorText.setPosition(cfg.winWidth*0.15+150,cfg.winHeight*0.65+10);
	authorTextShadow.setFont(cfg.fBold);
	authorTextShadow.setFillColor(sf::Color::Black);
	authorTextShadow.setCharacterSize(50);
	authorTextShadow.setPosition(cfg.winWidth*0.15+152,cfg.winHeight*0.65+12);

	// Change "title text" and it's shadow style
	titleText.setFont(cfg.fRegular);
	titleText.setCharacterSize(30);
	titleText.setPosition(cfg.winWidth*0.15+150,cfg.winHeight*0.65+70);
	titleTextShadow.setFont(cfg.fRegular);
	titleTextShadow.setFillColor(sf::Color::Black);
	titleTextShadow.setCharacterSize(30);
	titleTextShadow.setPosition(cfg.winWidth*0.15+152,cfg.winHeight*0.65+72);
	
	// Change "visualizer bars" style
	for(int i=0; i<barAmount; i++){
		barRect[i].setSize(sf::Vector2f(1, 10));
		barRect[i].setFillColor(sf::Color::White);
		barRect[i].setPosition(cfg.winWidth*0.15 + i * floor((cfg.winWidth*0.7-barAmount+1)/barAmount)+5, cfg.winHeight*0.65-20);
		barRectShadow[i].setSize(sf::Vector2f(1, 10));
		barRectShadow[i].setFillColor(sf::Color::Black);
		barRectShadow[i].setPosition(cfg.winWidth*0.15 + i * floor((cfg.winWidth*0.7-barAmount+1)/barAmount)+5+1, cfg.winHeight*0.65-20+1);
	}
	std::cout << "test" << std::endl;
}

void SpectrumComp::updateProgressBar(sf::Vector2f amount){
	progressBar.setLength(amount);
}

void SpectrumComp::updateProgressBarTime(HCHANNEL channelH){
	double time, duration;
	time = BASS_ChannelBytes2Seconds(channelH, BASS_ChannelGetPosition(channelH, BASS_POS_BYTE));
	duration = BASS_ChannelBytes2Seconds(channelH, BASS_ChannelGetLength(channelH, BASS_POS_BYTE));
	progressBar.setText(toHumanTime(time) + "/" + toHumanTime(duration));
}

void SpectrumComp::updateVisualizerBars(HCHANNEL channelH){
	BASS_ChannelGetData(channelH, fft, cfg.fftfreq);
	for(int i=0; i<barAmount; i++){
		float sum = 0;
		for(int j = 0; j<smoothBy-1; j++){
			smoothingBars[i][j] = smoothingBars[i][j+1];
			sum += smoothingBars[i][j];
		}
		smoothingBars[i][smoothBy-1] = fft[i];
		sum += smoothingBars[i][smoothBy-1];
		// Set value of each bar by formula: -1 (to flip upside down bar) * square root of -> divide sum of smoothing values by smoothing var * 60% - 1px of window's height
		bars[i] = -1.0f*sqrt(sum/smoothBy)*(cfg.winHeight*0.6f)-1.0f;
	}

	// Smooth all bars by taking average of neighbours and set their size
	for(int i=0; i<barAmount; i++){
		aveBars[i] = (bars[std::max(0, i - 1)] + bars[i] + bars[std::min(barAmount - 1, i + 1)]) / 3;
		barRect[i].setSize(sf::Vector2f(barWidth, biggerFloatOrDouble(aveBars[i],cfg.winHeight*-0.35)));
		barRectShadow[i].setSize(sf::Vector2f(barWidth, biggerFloatOrDouble(aveBars[i],cfg.winHeight*-0.35)));
	}
}

void SpectrumComp::updateAuthorAndTitle(){
	authorText.setString(author);
	authorTextShadow.setString(author);
	titleText.setString(title);
	titleTextShadow.setString(title);
}

void SpectrumComp::onWindowResizing(unsigned int winW, unsigned int winH){
	// Set new window's height and width to variable
	cfg.winWidth = winW;
	cfg.winHeight = winH;

	// Reposition/Resize elements
	progressBar.resize();

	authorText.setPosition(cfg.winWidth*0.15+150,cfg.winHeight*0.65+10);
	authorTextShadow.setPosition(cfg.winWidth*0.15+152,cfg.winHeight*0.65+12);
	titleText.setPosition(cfg.winWidth*0.15+150,cfg.winHeight*0.65+70);
	titleTextShadow.setPosition(cfg.winWidth*0.15+152,cfg.winHeight*0.65+72);
	albumCover.setPosition(cfg.winWidth*0.15,cfg.winHeight*0.65);

	for (int i = 0; i<barAmount; i++){
		barRect[i].setSize(sf::Vector2f(1, 10));
		barRect[i].setFillColor(sf::Color::White);
		barRect[i].setPosition(cfg.winWidth*0.15 + i * floor((cfg.winWidth*0.7-barAmount+1)/barAmount)+5, cfg.winHeight*0.65-20);
		barRectShadow[i].setSize(sf::Vector2f(1, 10));
		barRectShadow[i].setFillColor(sf::Color::Black);
		barRectShadow[i].setPosition(cfg.winWidth*0.15 + i * floor((cfg.winWidth*0.7-barAmount+1)/barAmount)+5+2, cfg.winHeight*0.65-20+2);
	}
	barWidth = floor((cfg.winWidth*0.7-barAmount+1)/barAmount)-5;
}

void SpectrumComp::setAuthor(std::wstring authorT){
	author = authorT;
	for(int i=0; i<author.size(); i++){ 
		author[i] = ((int)(author[i]) >= 97 && (int)author[i] <= 122) ? (author[i]&'_') : author[i]; 
	}
}

void SpectrumComp::setTitle(std::wstring titleT){
	title = titleT;
}

void SpectrumComp::onSongUpdate(double tmp_duration){
	progressBar.setTimeAndDuration(tmp_duration);
}

void SpectrumComp::draw(sf::RenderWindow &windowToDrawOn){
	windowToDrawOn.draw(albumCoverSprite,&cfg.shader_brightness);
	for(int i=0; i<barAmount; i++){
		windowToDrawOn.draw(barRectShadow[i]);
		windowToDrawOn.draw(barRect[i]);
	}
	windowToDrawOn.draw(albumCover);
	windowToDrawOn.draw(authorTextShadow);
	windowToDrawOn.draw(authorText);
	windowToDrawOn.draw(titleTextShadow);
	windowToDrawOn.draw(titleText);
	progressBar.draw(windowToDrawOn);
}

double SpectrumComp::onClickProgressBar(sf::Vector2i cords){
	return progressBar.onClick(cords);
}