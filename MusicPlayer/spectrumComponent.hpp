#pragma once
#include "config.hpp"
#include "song.hpp"
#include "button.hpp"
#include "GUI/screen.hpp"

class ProgressBar final {
	private:
		sf::Text progressBarTime;
		
		double duration;
	public:
		sf::RectangleShape progressBarBack, progressBarFront;
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
		const static short barAmount = 62; // Amount of bars
		unsigned short barSize = 10; // Size of bars
		const static unsigned short smoothBy = 19; // Amount of smoothing variables
		float barWidth = (cfg.winWidth*0.7f-static_cast<float>(barAmount)+1.0f)/static_cast<float>(barAmount)-5.0f;
		float bars[barAmount]; // Array holding bars values
		float smoothingBars[barAmount][smoothBy] = {}; // Array holding variables used for smoothing bars
		float aveBars[barAmount];

		// Sub Compontents
		ProgressBar progressBar;

		// Visualizer bars
		sf::RectangleShape barRect[barAmount];
		sf::RectangleShape barRectShadow[barAmount];

		// Artist and title text displays
		sf::Text titleText;
		sf::Text titleTextShadow;
		sf::Text artistText;
		sf::Text artistTextShadow;

		GUI::Button but_playSong = GUI::Button("Play Song", GUI::Position::POS_NORMAL, sf::Vector2f(10.0f, 10.0f), sf::Vector2f(10.0f, 5.0f), sf::Color(200, 200, 200));
		GUI::Button but_nextSong = GUI::Button("Next Song", GUI::Position::POS_NORMAL, sf::Vector2f(10.0f, 40.0f), sf::Vector2f(10.0f, 5.0f), sf::Color(200, 200, 200));
		GUI::Button but_prevSong = GUI::Button("Previous Song", GUI::Position::POS_NORMAL, sf::Vector2f(10.0f, 70.0f), sf::Vector2f(10.0f, 5.0f), sf::Color(200, 200, 200));
		GUI::Button but_chooseSong = GUI::Button("Open folder", GUI::Position::POS_BOTTOM, sf::Vector2f(10.0f, 50.0f), sf::Vector2f(10.0f, 5.0f), sf::Color(200, 200, 200));
	public:
		// turbo trio
		sf::Texture texture;
		sf::Sprite albumCoverSprite;
		sf::RectangleShape albumCover;

		// I still dont know this
		void onSongUpdate(double tmp_duration);
		double onClickProgressBar(sf::Vector2i cords);
		void fontReload();

		void onWindowResizing(float a_winWidth = cfg.winWidth, float a_winHeight = cfg.winHeight);
		void setVisualizerBars();
		void setProgressBarTime() noexcept;
		void setAuthorAndTitle(const song& a_song) noexcept;
		void setProgressBar(const sf::Vector2f& a_amount) noexcept;
		void reload();

		virtual void draw(sf::RenderWindow& a_win) final;
		
		SpectrumComp();
		~SpectrumComp(){};
};