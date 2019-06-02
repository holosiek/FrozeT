#pragma once

#include <SFML/Graphics.hpp>

enum View{
	None,
	Spectrum,
	Tracklist,
	Settings
};
namespace Screens{
	static View screenOnUse = View::Spectrum;

	class Screen{
		protected:
			Screen();
			~Screen();
		public:
			virtual void draw(sf::RenderWindow& a_win){};
	};
}



