#include "Bullet.hpp"

namespace rpf {
	Bullet::Bullet(int x, int y, int speed, ResourceHolder* rh) {
		setRadius(5);
		setTexture(&rh->bullet);
		x_speed = speed;
		setPosition(x, y);
	}

	void Bullet::update() {
		move(x_speed, 0);
	}
}