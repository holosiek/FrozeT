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
#include "button.hpp"
#include "logger.hpp"
#include "GUI/screen.hpp"
#include "GUI/window.hpp"


std::vector<GUI::Checkbox> buttonListOfDevices = {};

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

void SpectrumComp::fontReload(){
	but_nextSong.update();
	but_prevSong.update();
	but_playSong.update();
	but_chooseSong.update();
	for(size_t i=0; i<buttonListOfDevices.size(); i++){
		buttonListOfDevices[i].update();
	}
}

void SpectrumComp::reload(){
	// Change "album cover image" style
	albumCover.setSize(sf::Vector2f(128.0f, 128.0f));
	albumCover.setPosition(cfg.winWidth*0.15f, cfg.winHeight*0.65f);
	albumCover.setTexture(&texture);
	albumCover.setTextureRect(sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));

	// Change "artist text" and it's shadow style
	artistText.setFont(cfg.fBold);
	artistText.setFillColor(sf::Color::White);
	artistText.setCharacterSize(50u);
	artistText.setPosition(cfg.winWidth*0.15f+150.0f, cfg.winHeight*0.65f+10.0f);
	artistTextShadow.setFont(cfg.fBold);
	artistTextShadow.setFillColor(sf::Color::Black);
	artistTextShadow.setCharacterSize(50u);
	artistTextShadow.setPosition(cfg.winWidth*0.15f+152.0f, cfg.winHeight*0.65f+12.0f);

	// Change "title text" and it's shadow style
	titleText.setFont(cfg.fRegular);
	titleText.setFillColor(sf::Color::White);
	titleText.setCharacterSize(30u);
	titleText.setPosition(cfg.winWidth*0.15f+150.0f, cfg.winHeight*0.65f+70.0f);
	titleTextShadow.setFont(cfg.fRegular);
	titleTextShadow.setFillColor(sf::Color::Black);
	titleTextShadow.setCharacterSize(30u);
	titleTextShadow.setPosition(cfg.winWidth*0.15f+152.0f, cfg.winHeight*0.65f+72.0f);
	
	// Change "visualizer bars" style
	for(int i=0; i<barAmount; i++){
		barRect[i].setSize(sf::Vector2f(1.0f, 10.0f));
		barRect[i].setFillColor(sf::Color::White);
		barRect[i].setPosition(cfg.winWidth*0.15f+i*(cfg.winWidth*0.7f-barAmount+1.0f)/barAmount+5.0f, cfg.winHeight*0.65f-20.0f);
		barRectShadow[i].setSize(sf::Vector2f(1.0f, 10.0f));
		barRectShadow[i].setFillColor(sf::Color::Black);
		barRectShadow[i].setPosition(cfg.winWidth*0.15f+i*(cfg.winWidth*0.7f-barAmount+1.0f)/barAmount+6.0f, cfg.winHeight*0.65f-19.0f);
	}

	// Set texture
	texture.setSmooth(true);

	// Clear buttons vectors
	buttonListOfDevices.clear();

	// Push buttons
	BASS_DEVICEINFO info;
	BASS_SetConfig( BASS_CONFIG_UNICODE, true );
	for(int a = 1; BASS_GetDeviceInfo(a, &info); a++){
		buttonListOfDevices.emplace_back(GUI::Checkbox(info.name, GUI::POS_BOTTOM, sf::Vector2f(10.0f, 80.0f+a*26.0f), sf::Vector2f(10.0f, 5.0f), sf::Color(200, 200, 200)));
	}
	info = {};
	for(size_t i=0; i<buttonListOfDevices.size(); i++){
		BASS_GetDeviceInfo(i+1, &info);
		buttonListOfDevices[i].setSize(sf::Vector2f(350.0f, 24.0f));
		if(info.flags & BASS_DEVICE_DEFAULT){
			std::cout << "Test";
			buttonListOfDevices[i].setSelected();
		}
	}

	// Reload all buttons
	fontReload();

	Logger::log("INFO - player.cpp init()", "Appended buttons");
}

void SpectrumComp::onSongUpdate(double tmp_duration){
	progressBar.setTimeAndDuration(tmp_duration);
}

double SpectrumComp::onClickProgressBar(sf::Vector2i cords){
	return progressBar.onClick(cords);
}


/*
	###############################################
		Event related
	###############################################
*/

void SpectrumComp::onWindowResizing(float a_winWidth, float a_winHeight){
	// Set new window's height and width to variable
	cfg.winWidth = a_winWidth;
	cfg.winHeight = a_winHeight;

	// Reposition/Resize elements
	progressBar.resize();

	// Set new position of objects
	artistText.setPosition(cfg.winWidth*0.15f+150.0f, cfg.winHeight*0.65f+10.0f);
	artistTextShadow.setPosition(cfg.winWidth*0.15f+152.0f, cfg.winHeight*0.65f+12.0f);
	titleText.setPosition(cfg.winWidth*0.15f+150.0f, cfg.winHeight*0.65f+70.0f);
	titleTextShadow.setPosition(cfg.winWidth*0.15f+152.0f, cfg.winHeight*0.65f+72.0f);
	albumCover.setPosition(cfg.winWidth*0.15f, cfg.winHeight*0.65f);

	for(size_t i=0; i<barAmount; i++){
		barRect[i].setSize(sf::Vector2f(1.0f, 10.0f));
		barRect[i].setFillColor(sf::Color::White);
		barRect[i].setPosition(cfg.winWidth*0.15f+i*(cfg.winWidth*0.7f-barAmount+1.0f)/barAmount+5.0f, cfg.winHeight*0.65f-20.0f);
		barRectShadow[i].setSize(sf::Vector2f(1.0f, 10.0f));
		barRectShadow[i].setFillColor(sf::Color::Black);
		barRectShadow[i].setPosition(cfg.winWidth*0.15f+i*(cfg.winWidth*0.7f-barAmount+1.0f)/barAmount+6.0f, cfg.winHeight*0.65f-19.0f);
	}
	barWidth = (cfg.winWidth*0.7f-static_cast<float>(barAmount)+1.0f)/static_cast<float>(barAmount)-5.0f;
}

/*
	###############################################
		Misc.
	###############################################
*/

//Return string with length of 2
std::string toDoubleChars(std::string x){
	if(x.size() == 1){
		return "0" + x;
	} else {
		return x;
	}
}

// Change seconds to time in HH:MM:SS format
std::string changeTimeFormat(double& ti){
	std::string str;
	int timeInt = static_cast<int>(ti);
	int timeToCheck = timeInt / 3600;
	if (timeToCheck != 0){
		str += std::to_string(timeToCheck) + ":";
		timeInt -= timeToCheck * 3600;
		timeToCheck = timeInt / 60;
		str += toDoubleChars(std::to_string(timeToCheck)) + ":";
	} else {
		timeInt -= timeToCheck * 3600;
		timeToCheck = timeInt / 60;
		str += std::to_string(timeToCheck) + ":";
	}
	timeInt -= timeToCheck * 60;
	str += toDoubleChars(std::to_string(timeInt));
	return str;
}

/*
	###############################################
		Setters
	###############################################
*/

// Update artist and title name on display
void SpectrumComp::setAuthorAndTitle(const song& a_song) noexcept{
	// Update artist name
	artistText.setString(a_song.artist);
	artistTextShadow.setString(a_song.artist);

	// Update title name
	titleText.setString(a_song.title);
	titleTextShadow.setString(a_song.title);
}

// Update progress bar time to HH:MM:SS/HH:MM:SS format
void SpectrumComp::setProgressBarTime() noexcept{
	double time = BASS_ChannelBytes2Seconds(cfg.channel, BASS_ChannelGetPosition(cfg.channel, BASS_POS_BYTE));
	double duration = BASS_ChannelBytes2Seconds(cfg.channel, BASS_ChannelGetLength(cfg.channel, BASS_POS_BYTE));
	progressBar.setText(changeTimeFormat(time) + "/" + changeTimeFormat(duration));
}

// Set progress bar width
void SpectrumComp::setProgressBar(const sf::Vector2f& a_amount) noexcept{
	progressBar.progressBarFront.setSize(a_amount);
}

// Update visualizer bars
void SpectrumComp::setVisualizerBars(){
	// Get channel data to array
	float fft[2048];
	BASS_ChannelGetData(cfg.channel, fft, cfg.fftfreq);
	
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
				barRect[i].setSize(sf::Vector2f(barWidth, std::max(aveBars[i], cfg.winHeight*-0.35f)));
				barRectShadow[i].setSize(sf::Vector2f(barWidth, std::max(aveBars[i], cfg.winHeight*-0.35f)));
			}
			break;

		// Avarage of 3
		default:
		case 1:
			for(short i=0; i<barAmount; i++){
				aveBars[i] = (bars[std::max(0, i - 1)] + bars[i] + bars[std::min(barAmount - 1, i + 1)]) / 3;
				barRect[i].setSize(sf::Vector2f(barWidth, std::max(aveBars[i], cfg.winHeight*-0.35f)));
				barRectShadow[i].setSize(sf::Vector2f(barWidth, std::max(aveBars[i], cfg.winHeight*-0.35f)));
			}
			break;

		// Avarage of 5
		case 2:
			for(short i=0; i<barAmount; i++){
				aveBars[i] = (bars[std::max(0, i - 2)] + bars[std::max(0, i - 1)] + bars[i] + bars[std::min(barAmount - 1, i + 1)] + bars[std::min(barAmount - 1, i + 2)]) / 5;
				barRect[i].setSize(sf::Vector2f(barWidth, std::max(aveBars[i], cfg.winHeight*-0.35f)));
				barRectShadow[i].setSize(sf::Vector2f(barWidth, std::max(aveBars[i], cfg.winHeight*-0.35f)));
			}
			break;

		// Experimental
		case 3:
			for(short i=0; i<barAmount; i++){
				aveBars[i] = (bars[std::max(0, i - 2)]+bars[std::max(0, i - 1)]+bars[i]+bars[i]+bars[i]+bars[std::min(barAmount - 1, i + 1)]+bars[std::min(barAmount - 1, i + 2)])/5;
				barRect[i].setSize(sf::Vector2f(barWidth, std::max(aveBars[i], cfg.winHeight*-0.35f)));
				barRectShadow[i].setSize(sf::Vector2f(barWidth, std::max(aveBars[i], cfg.winHeight*-0.35f)));
			}
			break;
	}
}

/*
	###############################################
		Constructor, Decontructor & draw func.
	###############################################
*/
SpectrumComp::SpectrumComp(){}

void SpectrumComp::draw(sf::RenderWindow& a_win){
	// Clear window
	a_win.clear(cfg.winBackground);

	// Get time and duration of track and set size of prograss bar
	setProgressBar(sf::Vector2f(((cfg.winWidth - 20.0f)*BASS_ChannelGetPosition(cfg.channel, BASS_POS_BYTE)/BASS_ChannelGetLength(cfg.channel, BASS_POS_BYTE)), 6.0f));
	
	// Change progress bar text and position
	setProgressBarTime();

	// Take channel data, change it into visualizer bar data and smooth each bar
	setVisualizerBars();

	// Handle events
	sf::Event event;
	while(a_win.pollEvent(event)){
		switch(event.type){
			// On window close
			case sf::Event::Closed:
				a_win.close();
				break;

			// On window resize
			case sf::Event::Resized:
				a_win.setView(sf::View(sf::FloatRect(0.0f, 0.0f, event.size.width, event.size.height)));
				onWindowResizing(static_cast<float>(event.size.width), static_cast<float>(event.size.height));
				fontReload();
				Player::refreshAlbum();
				break;

			// On key press
			case sf::Event::KeyPressed:
				switch(event.key.code){
					case sf::Keyboard::F11:
						GUI::Window::windowSetFullscreen(a_win.getSystemHandle());
						break;
					case sf::Keyboard::H:
						cfg.drawHUD = !cfg.drawHUD;
						break;
					case sf::Keyboard::Space:
						Player::pauseSong();
						break;
					default:
						break;
				}

			// On mouse press
			case sf::Event::MouseButtonPressed:
				if(event.mouseButton.button == 0){
					sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
					for(size_t i=0; i<buttonListOfDevices.size(); i++){
						if(buttonListOfDevices[i].checkIfClicked(mousePos)){
							Player::changeChannel(i+1);
							for(size_t j=0; j<buttonListOfDevices.size(); j++){
								if(j!=i){
									buttonListOfDevices[j].updateSelected();
								}
							}
							break;
						}
					}
					if(but_playSong.checkIfClicked(mousePos)){
						Player::pauseSong();
					}
					if(but_nextSong.checkIfClicked(mousePos)){
						Player::playNext();
					}
					if(but_prevSong.checkIfClicked(mousePos)){
						Player::playPrevious();
					}
					if(but_chooseSong.checkIfClicked(mousePos)){
						Player::takeMusicFromFolder(L"Open folder which holds music");
					}
				}
				break;

			case sf::Event::MouseEntered:
				cfg.isClickedWindow = true;
				break;

			case sf::Event::MouseLeft:
				cfg.isClickedWindow = false;
				break;

			// On lost window focus
			case sf::Event::LostFocus:
				GUI::Window::saveMode(true, a_win);
				break;

			// On gain window focus
			case sf::Event::GainedFocus:
				GUI::Window::saveMode(false, a_win);
				break;

			case sf::Event::MouseMoved: {
				sf::Vector2i mousePos(sf::Mouse::getPosition(a_win).x, sf::Mouse::getPosition(a_win).y);
				but_nextSong.checkIfHover(mousePos);
				but_prevSong.checkIfHover(mousePos);
				but_playSong.checkIfHover(mousePos);
				but_chooseSong.checkIfHover(mousePos);
				for(size_t i=0; i<buttonListOfDevices.size(); i++){
					buttonListOfDevices[i].checkIfHover(mousePos);
				}
				break;
			}
			default:
				break;
		}
	}

	a_win.draw(albumCoverSprite, &cfg.shader_brightness);
	for(size_t i=0; i<barAmount; i++){
		a_win.draw(barRectShadow[i]);
		a_win.draw(barRect[i]);
	}
	a_win.draw(albumCover);
	a_win.draw(artistTextShadow);
	a_win.draw(artistText);
	a_win.draw(titleTextShadow);
	a_win.draw(titleText);
	progressBar.draw(a_win);
	
	if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && cfg.isClickedWindow){
		sf::Vector2i mouseCords = sf::Mouse::getPosition(a_win);
		double getClicked = onClickProgressBar(sf::Vector2i(mouseCords.x,mouseCords.y));
		if(getClicked != -1){
			BASS_ChannelSetPosition(cfg.channel, BASS_ChannelSeconds2Bytes(cfg.channel, getClicked), BASS_POS_BYTE);
		}
	}

	if(cfg.drawHUD){
		but_nextSong.draw(a_win);
		but_prevSong.draw(a_win);
		but_playSong.draw(a_win);
		but_chooseSong.draw(a_win);
		for(size_t i=0; i<buttonListOfDevices.size(); i++){
			buttonListOfDevices[i].draw(a_win);
		}
	}

	a_win.display();
}