#pragma once

#include "SFML/Graphics.hpp"
#include "ResourceHolder.hpp"

namespace rpf {
	class Bullet : public sf::CircleShape {
	public:
		Bullet(int x, int y, int speed, ResourceHolder* rh);
		~Bullet() = default;
		void update();
		int getSpeed() { return x_speed; }
		int getX() { return x; }
		int getY() { return y; }
	private:
		int x_speed = 0;
		int x = 0, y = 0;
	};
}