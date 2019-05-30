#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <memory>

class Screen {
	protected:
		Screen();
		~Screen();
	public:
	virtual void draw(sf::RenderWindow& a_win){};
};