#pragma once
#include "config.hpp"
#include "song.hpp"
#include "GUI/screen.hpp"

class ProgressBar final {
	private:
		sf::Text progressBarTime;
		
		double duration;
	public:
	sf::RectangleShape progressBarBack, progressBarFront;
		void setLength(sf::Vector2f amount);
		void setText(std::string text);
		void setTimeAndDuration(double tmp_duration);
		void resize();
		void draw(sf::RenderWindow &windowToDrawOn);
		double onClick(sf::Vector2i cords);
		ProgressBar();
};

class SpectrumComp final : public Screens::Screen{
	private:
		// Bars
		const static size_t barAmount = 62;                    // Amount of bars
		unsigned short barSize = 10;                                   // Size of bars
		const static unsigned short smoothBy = 19;                      // Amount of smoothing variables
		int barWidth = static_cast<int>(floor((cfg.winWidth*0.7-barAmount+1)/barAmount)-5);

		//float fft[2048];                                               // FFT array (1/2 of freq of samples)
		float bars[barAmount];                                         // Array holding bars values
		float smoothingBars[barAmount][smoothBy] = {};              // Array holding variables used for smoothing bars
		float aveBars[barAmount];
		// Shapes
		ProgressBar progressBar;                                       // Whole Progress Bar
		sf::RectangleShape barRect[barAmount];                         // Visualizer bars
		sf::RectangleShape barRectShadow[barAmount];                   // Visualizer bars shadow
		sf::Text titleText; sf::Text titleTextShadow;                  // Title text and it's shadow
		sf::Text authorText; sf::Text authorTextShadow;                // Author text and it's shadow

	public:
		// turbo trio
		sf::Texture texture;
		sf::Sprite albumCoverSprite;
		sf::RectangleShape albumCover;
		//
		void updateProgressBar(const sf::Vector2f& amount);
		void updateAuthorAndTitle(song& a_song);
		void onWindowResizing(unsigned int winW = cfg.winWidth, unsigned int winH = cfg.winHeight);
		void onSongUpdate(double tmp_duration);
		double onClickProgressBar(sf::Vector2i cords);
		


		// F I X E D a bit

		SpectrumComp();
		virtual void draw(sf::RenderWindow& a_win) final;

		// F I X E D
		void updateProgressBarTime();
		void updateVisualizerBars();
};