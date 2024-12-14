#pragma once

#include "SFML/Graphics.hpp"
#include "ResourceHolder.hpp"

namespace rpf {
	class Coin {
	public:
		Coin(ResourceHolder* rh, int x, int y, int id);
		~Coin() = default;
		void update();
		int getScore() { return 100; }
		int getId() { return id; }
		sf::Sprite& getDrawable() { return spr; }
	private:
		int id;
		const int anim_speed = 5;
		const int o_coin_size = 128;
		int anim_index = 0;
		ResourceHolder* rh;
		sf::Sprite spr;
	};
}