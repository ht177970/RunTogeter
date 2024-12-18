#include "Player.hpp"
#include "Game.hpp"
#include "Core.hpp"
#include <iostream>

namespace rpf {
	Player::Player(ResourceHolder* rh, Map* m, Game* _game, int id) {
		this->id = id;
		this->rh = rh;
		this->back_map = m;
		this->_game = _game;
	}

	void Player::spawn(int x, int y) {
		lastspawn_x = x * rh->tile_size;
		lastspawn_y = y * rh->tile_size;
		/*lastspawn_x += rh->view->getCenter().x - rh->s_width / 2;
		lastspawn_y += rh->view->getCenter().y - rh->s_height / 2;*/
		this->current_sprite.setPosition(lastspawn_x, lastspawn_y);
		lastview_x = rh->view->getCenter().x;
		lastview_y = rh->view->getCenter().y;
	}

	void Player::NEWspawn(int x, int y) {
		this->current_sprite.move(x, y);
		lastspawn_x = this->current_sprite.getPosition().x;
		lastspawn_y = this->current_sprite.getPosition().y;
		lastview_x = rh->view->getCenter().x;
		lastview_y = rh->view->getCenter().y;
	}

	void Player::KeyPress(sf::Keyboard::Key k) {
		switch (k) {
		case sf::Keyboard::A://left
			x_speed = -unit_speed;
			left_key = true;
			break;
		case sf::Keyboard::D:
			x_speed = unit_speed;
			right_key = true;
			break;
		case sf::Keyboard::W:
			w_key = true;
			break;
		case sf::Keyboard::Space:
			space_key = true;
			break;
		}
	}

	void Player::KeyRelease(sf::Keyboard::Key k) {
		switch (k) {
		case sf::Keyboard::A:
			if (right_key)
				x_speed = unit_speed;
			else
				x_speed = 0;
			left_key = false;
			break;
		case sf::Keyboard::D:
			if (left_key)
				x_speed = -unit_speed;
			else
				x_speed = 0;
			right_key = false;
			break;
		case sf::Keyboard::W:
			w_key = false;
			break;
		case sf::Keyboard::Space:
			space_key = false;
			break;
		}
	}

	void Player::update() {
		if (!killed) {
			jump();
			shoot();
			check_face();
			move_map();
			update_current_tiles();
			check_move_x();
			update_current_tiles();
			check_move_y();
			check_out();
			update_spr();
		}
		else {
			death_anim();
		}
	}

	void Player::jump() {
		if (w_key && y_speed == 0) {
			update_current_tiles();
			int speed = y_speed;
			for (int i = current_tile_x_left; i <= current_tile_x_right; i++)
				if (!_game->is_empty_block(i, current_tile_y_bottom + 1)) {
					speed = unit_speed * -2.5;
					rh->jump.play();
					break;
				}
			y_speed = speed;
		}
	}

	void Player::shoot() {
		if (space_key && !shooting) {
			shooting = true;
			anim_index = 0;
			_game->player_shot(current_sprite.getGlobalBounds(), flip);
			rh->shot.play();
		}
	}

	void Player::check_face() {
		if (x_speed > 0 && flip)
			flip = !flip;
		else if (x_speed < 0 && !flip)
			flip = !flip;
	}

	void Player::move_map() {
		if (((x_speed > 0 && current_sprite.getGlobalBounds().left > rh->view->getCenter().x + 100) || (x_speed < 0 && current_sprite.getGlobalBounds().left < rh->view->getCenter().x - 300)))
		{
			rh->view->move(x_speed, 0);
			back_map->move(x_speed * 5 / 7, 0);
			rh->back_sprite.move(x_speed * 5 / 7, 0);
		}
	}

	void Player::check_move_x() {
		int dx = x_speed > 0 ? current_tile_x_right + 1 : current_tile_x_left - 1;
		float ix = x_speed > 0 ? -(current_sprite.getGlobalBounds().left + current_sprite.getGlobalBounds().width) : rh->tile_size - current_sprite.getGlobalBounds().left;
		for (int i = current_tile_y_top; i <= current_tile_y_bottom; i++)
		{
			if (!_game->is_empty_block(dx, i))
			{
				float dist_to_obstacle = _game->get_cord_of_tile(dx, i).x + ix;
				if (x_speed > 0 && x_speed > dist_to_obstacle) {
					current_sprite.move(dist_to_obstacle - 1, 0);
					return;
				}
				else if (x_speed <= 0 && x_speed <= dist_to_obstacle) {
					current_sprite.move(dist_to_obstacle + 1, 0);
					return;
				}
			}
		}
		current_sprite.move(x_speed, 0);
	}

	void Player::check_move_y() {
		y_speed += g;
		int dy = y_speed > 0 ? 1 : -1;
		for (int i = current_tile_x_left; i <= current_tile_x_right; i++)
		{
			if (!_game->is_empty_block(i, current_tile_y_bottom + dy))
			{
				float dist_to_obstacle = _game->get_cord_of_tile(i, current_tile_y_bottom + dy).y - (current_sprite.getGlobalBounds().top + current_sprite.getGlobalBounds().height);
				y_speed = (y_speed < dist_to_obstacle) ? y_speed : (dist_to_obstacle - 1);
				if (dist_to_obstacle < 2) {
					y_speed = 0;
					return;
				}
			}
		}
		current_sprite.move(0, y_speed);
	}

	void Player::check_out() {
		if (current_sprite.getGlobalBounds().top > rh->s_height) {
			killed = true;
			anim_index = x_speed = y_speed = 0;
			rh->fall.play();
		}
	}

	void Player::update_spr() {
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
	}

	void Player::dead() {
		killed = true;
		anim_index = x_speed = y_speed = 0;
		rh->death.play();
	}

	void Player::death_anim() {
		if (anim_index == 10 * ani_speed * 2) {
			if (life && delay++ == dead_delay) {
				if (!Core::CORE->GAME) {
					life--;
					if (life == 0)
						return;
				}
				delay = anim_index = x_speed = y_speed = 0;
				killed = flip = shooting = false;
				current_sprite.setPosition(lastspawn_x, lastspawn_y);
				rh->view->setCenter(lastview_x, lastview_y);
			}
			return;
		}
		current_sprite.setTexture(rh->player_die[anim_index++ / (ani_speed * 2)]);
		current_sprite.setTextureRect(getFaceRect(current_sprite.getTexture()->getSize()));
		current_sprite.setScale(0.2 * rh->trans, 0.2 * rh->trans);
	}

	sf::IntRect Player::getFaceRect(sf::Vector2u size) {
		if (flip)
			return sf::IntRect(size.x, 0, -1 * size.x, fixRunningHeight(size.y));
		else
			return sf::IntRect(0, 0, size.x, fixRunningHeight(size.y));
	}

	int Player::fixRunningHeight(int height) {
		return height == 447 ? 421 : height;
	}

	bool Player::onGround() {
		for (int i = current_tile_x_left; i <= current_tile_x_right; i++)
			if (!_game->is_empty_block(i, current_tile_y_bottom + 1))
				return true;
		return false;
	}

	void Player::update_current_tiles()
	{
		current_tile_x_left = (current_sprite.getGlobalBounds().left - _game->getMap().getPosition().x) / (rh->tile_size);
		current_tile_x_right = ((current_sprite.getGlobalBounds().left + current_sprite.getGlobalBounds().width - 4) - _game->getMap().getPosition().x) / (rh->tile_size);
		current_tile_y_top = (current_sprite.getGlobalBounds().top - _game->getMap().getPosition().y) / (rh->tile_size);
		current_tile_y_bottom = ((current_sprite.getGlobalBounds().top + current_sprite.getGlobalBounds().height) - _game->getMap().getPosition().y) / (rh->tile_size);
	}
}