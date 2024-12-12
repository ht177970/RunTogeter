#pragma once

#include "SFML/Graphics.hpp"
#include "ResourceHolder.hpp"

namespace rpf {
	class Portal {
	public:
		Portal(ResourceHolder* rh, int x, int y);
		void update();
		sf::Sprite& getDrawable() { return spr; }
	private:
		const int size = 64;
		const int speed = 10;
		int anim_index = 0;
		sf::Texture m_tileset;
		sf::Sprite spr;
	};
}