#include "Coin.hpp"

namespace rpf {
	Coin::Coin(ResourceHolder* rh, int x, int y, int id) {
		this->id = id;
		this->rh = rh;
		this->update();
		spr.setPosition(x * rh->tile_size, y * rh->tile_size);
	}

	void Coin::update() {
		if (anim_index >= 6 * anim_speed)
			anim_index = 0;
		spr.setTexture(rh->coin[anim_index++ / anim_speed]);
		spr.setScale(static_cast<float>(rh->tile_size) / o_coin_size, static_cast<float>(rh->tile_size) / o_coin_size);
	}
}