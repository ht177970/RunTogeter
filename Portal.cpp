#include "Portal.hpp"

namespace rpf {
	Portal::Portal(ResourceHolder* rh, int x, int y) {
		if (!m_tileset.loadFromFile("img/Portal.png"))
			return;
		spr.setTexture(m_tileset);
		update();
		spr.setScale(2, 2);
		spr.setOrigin(size / 4, size / 2);
		spr.setPosition(x * rh->tile_size, y * rh->tile_size);
	}

	void Portal::update() {
		spr.setTextureRect(sf::IntRect((anim_index/speed) * size, 0, size, size));
		if (++anim_index == 8 * speed)
			anim_index = 0;
	}
}