#pragma once

#include "SFML/Graphics.hpp"

namespace rpf {
	class Render {
	public:
		virtual void update() = 0;
		virtual void handleEvent(sf::Event e) = 0;
	};
}