#pragma once
#include <SFML/Window.hpp>

namespace GUI{
	namespace Window{
		void windowSetFullscreen(sf::WindowHandle a_handle);
		void saveMode(const bool& a_state, sf::Window& a_win);
	}
}