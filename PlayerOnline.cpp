#include "PlayerOnline.hpp"
#include "Game.hpp"
#include <iostream>

namespace rpf {
	PlayerOnline::PlayerOnline(ResourceHolder* rh, int id) {
		this->id = id;
		this->rh = rh;
	}

	/*void PlayerOnline::spawn(int x, int y) {
		lastspawn_x = x * rh->tile_size * rh->trans;
		lastspawn_y = y * rh->tile_size * rh->trans;
		this->current_sprite.setPosition(x * rh->tile_size * rh->trans, y * rh->tile_size * rh->trans);
		lastview_x = rh->view->getCenter().x;
		lastview_y = rh->view->getCenter().y;
	}

	void PlayerOnline::NEWspawn(int x, int y) {
		this->current_sprite.move(x, y);
		lastspawn_x = this->current_sprite.getPosition().x;
		lastspawn_y = this->current_sprite.getPosition().y;
		lastview_x = rh->view->getCenter().x;
		lastview_y = rh->view->getCenter().y;
	}*/

	void PlayerOnline::update() {
		if (!killed) {
			check_face();
			update_spr();
		}
		else {
			death_anim();
		}
	}

	void PlayerOnline::check_face() {
		if (x_speed > 0 && flip)
			flip = !flip;
		else if (x_speed < 0 && !flip)
			flip = !flip;
	}

	void PlayerOnline::update_spr() {//Known that there is no any anim for now
		if (shooting)
		{
			current_sprite.setTexture(rh->shooting[anim_index++ / ani_speed]);
			if (anim_index == 10 * ani_speed)
			{
				shooting = !shooting;
				anim_index = 0;
			}
		}
		else if (y_speed != 0 || !onGround())
		{
			current_sprite.setTexture(rh->jump_gun);
		}
		else if (x_speed != 0 && y_speed == 0)
		{
			current_sprite.setTexture(rh->gun_run[anim_index++ / ani_speed]);
			anim_index %= 10 * ani_speed;
		}
		else
		{
			current_sprite.setTexture(rh->player_idle[anim_index++ / ani_speed]);
			anim_index %= 10 * ani_speed;
		}
		current_sprite.setTextureRect(getFaceRect(current_sprite.getTexture()->getSize()));
		current_sprite.setScale(0.2 * rh->trans
			* (1.f * rh->player_idle[0].getSize().x / abs(current_sprite.getTextureRect().width)),
			0.2 * rh->trans
			* (1.f * rh->player_idle[0].getSize().y / current_sprite.getTextureRect().height));
		current_sprite.setOrigin(0, current_sprite.getTextureRect().height);//LeftDown
		current_sprite.setColor(sf::Color(255, 255, 255, 128));
	}

	void PlayerOnline::dead() {
		killed = true;
		anim_index = x_speed = y_speed = 0;
		rh->death.play();
	}

	void PlayerOnline::death_anim() {
		if (anim_index == 10 * ani_speed * 2) {
			if (delay++ == dead_delay) {
				delay = anim_index = x_speed = y_speed = 0;
				killed = flip = shooting = false;
			}
			return;
		}
		current_sprite.setTexture(rh->player_die[anim_index++ / (ani_speed * 2)]);
		current_sprite.setTextureRect(getFaceRect(current_sprite.getTexture()->getSize()));
		current_sprite.setScale(0.2 * rh->trans, 0.2 * rh->trans);
	}

	sf::IntRect PlayerOnline::getFaceRect(sf::Vector2u size) {
		if (flip)
			return sf::IntRect(size.x, 0, -1 * size.x, fixRunningHeight(size.y));
		else
			return sf::IntRect(0, 0, size.x, fixRunningHeight(size.y));
	}

	int PlayerOnline::fixRunningHeight(int height) {
		return height == 447 ? 421 : height;
	}

	bool PlayerOnline::onGround() {
		return true;//TODO

		/*for (int i = current_tile_x_left; i <= current_tile_x_right; i++)
			if (!_game->is_empty_block(i, current_tile_y_bottom + 1))
				return true;
		return false;*/
	}
}