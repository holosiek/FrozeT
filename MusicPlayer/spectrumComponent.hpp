#ifndef SPECTRUMCOMPONENT
#define SPECTRUMCOMPONENT

#include "config.hpp"
class ProgressBar {
	private:
		sf::Text progressBarTime;
		sf::RectangleShape progressBarBack, progressBarFront;
		double duration;
	public:
		void setLength(sf::Vector2f amount);
		void setText(std::string text);
		void setTimeAndDuration(double tmp_duration);
		void resize();
		void draw(sf::RenderWindow &windowToDrawOn);
		double onClick(sf::Vector2i cords);
		ProgressBar();
};

class SpectrumComp {
	private:
		// Bars
		const static unsigned short barAmount = 62;                    // Amount of bars
		unsigned short barSize = 10;                                   // Size of bars
		const static unsigned short smoothBy = 19;                      // Amount of smoothing variables
		int barWidth = floor((cfg.winWidth*0.7-barAmount+1)/barAmount)-5;

		float fft[2048];                                               // FFT array (1/2 of freq of samples)
		float bars[barAmount];                                         // Array holding bars values
		float smoothingBars[barAmount][smoothBy] = { 0 };              // Array holding variables used for smoothing bars
		float aveBars[barAmount];
	    // Music variables
	    std::wstring title, author;                                    // Variables holding title and author of music
		// Shapes
		ProgressBar progressBar;                                       // Whole Progress Bar
		sf::RectangleShape barRect[barAmount];                         // Visualizer bars
		sf::RectangleShape barRectShadow[barAmount];                   // Visualizer bars shadow
		sf::Text titleText; sf::Text titleTextShadow;                  // Title text and it's shadow
		sf::Text authorText; sf::Text authorTextShadow;                // Author text and it's shadow

	public:
		sf::Texture texture;                                           // Texture of album cover image
		sf::Sprite albumCoverSprite;                                   // Sprite of album cover image
		sf::RectangleShape albumCover;                                 // Album cover image near title and author
		
		void draw(sf::RenderWindow &windowToDrawOn);
		void updateProgressBar(sf::Vector2f amount);
		void updateProgressBarTime(HCHANNEL channelH);
		void updateVisualizerBars(HCHANNEL channelH);
		void updateAuthorAndTitle();
		void onWindowResizing(unsigned int winW = cfg.winWidth, unsigned int winH = cfg.winHeight);
		void setAuthor(std::wstring authorT);
		void setTitle(std::wstring titleT);
		void onSongUpdate(double tmp_duration);
		double onClickProgressBar(sf::Vector2i cords);
		SpectrumComp();
};

#endif SPECTRUMCOMPONENT