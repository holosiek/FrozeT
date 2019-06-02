#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "bass.h"
#include "spectrumComponent.hpp"
#include "config.hpp"
#include "mymisc.hpp"
#include "player.hpp"
#include "song.hpp"
#include "GUI/screen.hpp"

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
	// Change "album cover image" style
	albumCover.setSize(sf::Vector2f(128.0f, 128.0f));
	albumCover.setPosition(static_cast<float>(cfg.winWidth*0.15), static_cast<float>(cfg.winHeight*0.65));
	albumCover.setTexture(&texture);
	albumCover.setTextureRect(sf::IntRect(0,0,albumCover.getTexture()->getSize().x, albumCover.getTexture()->getSize().y));

	// Change "artist text" and it's shadow style
	authorText.setFont(cfg.fBold);
	authorText.setCharacterSize(50u);
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
}

void SpectrumComp::updateProgressBar(const sf::Vector2f& amount){
	progressBar.progressBarFront.setSize(amount);
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

void SpectrumComp::onSongUpdate(double tmp_duration){
	progressBar.setTimeAndDuration(tmp_duration);
}

double SpectrumComp::onClickProgressBar(sf::Vector2i cords){
	return progressBar.onClick(cords);
}

/*
	F I X E D
*/

void SpectrumComp::updateAuthorAndTitle(song& a_song){
	// Update artist name
	authorText.setString(a_song.artist);
	authorTextShadow.setString(a_song.artist);

	// Update title name
	titleText.setString(a_song.title);
	titleTextShadow.setString(a_song.title);
}

void SpectrumComp::updateProgressBarTime(){
	double time = BASS_ChannelBytes2Seconds(Player::channel, BASS_ChannelGetPosition(Player::channel, BASS_POS_BYTE));
	double duration = BASS_ChannelBytes2Seconds(Player::channel, BASS_ChannelGetLength(Player::channel, BASS_POS_BYTE));
	progressBar.setText(toHumanTime(time) + "/" + toHumanTime(duration));
}

void SpectrumComp::updateVisualizerBars(){
	// Get channel data to array
	float fft[2048];
	BASS_ChannelGetData(Player::channel, fft, cfg.fftfreq);
	
	// Smooth by taking multiple values of simple bar and taking avarage of them
	for(size_t i=0; i<barAmount; i++){
		float sum = 0;
		for(int j=0; j<smoothBy-1; j++){
			smoothingBars[i][j] = smoothingBars[i][j+1];
			sum += smoothingBars[i][j];
		}
		smoothingBars[i][smoothBy-1] = fft[i];
		sum += smoothingBars[i][smoothBy-1];
		// Set value of each bar by formula: -1 (to flip upside down bar) * square root of -> divide sum of smoothing values by smoothing var * 60% - 1px of window's height
		bars[i] = -1.0f*sqrt(sum/smoothBy)*(cfg.winHeight*0.6f)-1.0f;
	}

	// Smooth all bars by taking average of neighbours and set their size
	switch(cfg.smoothLevel){
		// No avarage
		case 0:
			for(size_t i=0; i<barAmount; i++){
				aveBars[i] = bars[i];
				barRect[i].setSize(sf::Vector2f(barWidth, biggerFloatOrDouble(aveBars[i], cfg.winHeight*-0.35)));
				barRectShadow[i].setSize(sf::Vector2f(barWidth, biggerFloatOrDouble(aveBars[i], cfg.winHeight*-0.35)));
			}
			break;

		// Avarage of 3
		default:
		case 1:
			for(size_t i=0; i<barAmount; i++){
				aveBars[i] = (bars[std::max(0u, i - 1)] + bars[i] + bars[std::min(barAmount - 1, i + 1)]) / 3;
				barRect[i].setSize(sf::Vector2f(barWidth, biggerFloatOrDouble(aveBars[i], cfg.winHeight*-0.35)));
				barRectShadow[i].setSize(sf::Vector2f(barWidth, biggerFloatOrDouble(aveBars[i], cfg.winHeight*-0.35)));
			}
			break;

		// Avarage of 5
		case 2:
			for(size_t i=0; i<barAmount; i++){
				aveBars[i] = (bars[std::max(0u, i - 2)] + bars[std::max(0u, i - 1)] + bars[i] + bars[std::min(barAmount - 1, i + 1)] + bars[std::min(barAmount - 1, i + 2)]) / 5;
				barRect[i].setSize(sf::Vector2f(barWidth, biggerFloatOrDouble(aveBars[i], cfg.winHeight*-0.35)));
				barRectShadow[i].setSize(sf::Vector2f(barWidth, biggerFloatOrDouble(aveBars[i], cfg.winHeight*-0.35)));
			}
			break;

		// Experimental
		case 3:
			for(size_t i=0; i<barAmount; i++){
				aveBars[i] = (bars[std::max(0u, i - 2)]+bars[std::max(0u, i - 1)]+bars[i]+bars[i]+bars[i]+bars[std::min(barAmount - 1, i + 1)]+bars[std::min(barAmount - 1, i + 2)])/5;
				barRect[i].setSize(sf::Vector2f(barWidth, biggerFloatOrDouble(aveBars[i], cfg.winHeight*-0.35)));
				barRectShadow[i].setSize(sf::Vector2f(barWidth, biggerFloatOrDouble(aveBars[i], cfg.winHeight*-0.35)));
			}
			break;
	}
	
}

void SpectrumComp::draw(sf::RenderWindow& a_win){
	// Clear window
	a_win.clear(cfg.winBackground);

	// Get time and duration of track and set size of prograss bar
	updateProgressBar(sf::Vector2f(static_cast<float>((cfg.winWidth - 20)*BASS_ChannelGetPosition(Player::channel, BASS_POS_BYTE) / BASS_ChannelGetLength(Player::channel, BASS_POS_BYTE)), 6.0f));
	
	// Change progress bar text and position
	updateProgressBarTime();

	// Take channel data, change it into visualizer bar data and smooth each bar
	updateVisualizerBars();

		a_win.draw(albumCoverSprite,&cfg.shader_brightness);
		for(int i=0; i<barAmount; i++){
			a_win.draw(barRectShadow[i]);
			a_win.draw(barRect[i]);
		}
		a_win.draw(albumCover);
	a_win.draw(authorTextShadow);
	a_win.draw(authorText);
	a_win.draw(titleTextShadow);
	a_win.draw(titleText);
		progressBar.draw(a_win);

	
	//a_win.display();
}