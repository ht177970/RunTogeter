#include "Enemy.hpp"
#include "Game.hpp"

namespace rpf {
	Enemy::Enemy(ResourceHolder* rh, Game* g, int ix, int iy, int id) {
		this->id = id;
		this->rh = rh;
		this->ix = ix;
		this->iy = iy;
		this->_game = g;
		this->update();
		spr.setScale(0.09, 0.09);
		spr.setOrigin(0, 64);
		spr.setPosition(ix * rh->tile_size, iy * rh->tile_size);
	}

	void Enemy::update() {
		if (killed) {
			anim_index++;
			spr.setTexture(rh->enemy_die);
			spr.move(0.5, -0.1);
			return;
		}
		if (anim_index >= 2 * anim_speed)
			anim_index = 0;
		spr.setTexture(rh->enemy[anim_index++ / anim_speed]);
		if (AI) {
			update_current_tiles();
			check_move_x();
			spr.setTextureRect(getFaceRect(spr.getTexture()->getSize()));
		}
	}

	void Enemy::dead() {
		killed = true;
		anim_index = 0;
	}

	void Enemy::check_move_x() {
		x_speed = flip ? -unit_speed : unit_speed;
		int dx = x_speed > 0 ? current_tile_x_right + 1 : current_tile_x_left - 1;
		double ix = x_speed > 0 ? -(spr.getGlobalBounds().left + spr.getGlobalBounds().width) : rh->tile_size - spr.getGlobalBounds().left;
		for (int i = current_tile_y_top; i <= current_tile_y_bottom; i++)
		{
			if (!_game->is_empty_block(dx, i))
			{
				double dist_to_obstacle = _game->get_cord_of_tile(dx, i).x + ix;
				if (x_speed > 0 && x_speed > dist_to_obstacle) {
					spr.move(dist_to_obstacle - 1, 0);
					flip = !flip;
					return;
				}
				else if (x_speed <= 0 && x_speed <= dist_to_obstacle) {
					spr.move(dist_to_obstacle + 1, 0);
					flip = !flip;
					return;
				}
			}
		}
		if (!_game->is_empty_block(flip ? current_tile_x_left : current_tile_x_right, current_tile_y_bottom + 1))
			spr.move(x_speed, 0);
		else
			flip = !flip;
	}

	void Enemy::update_current_tiles()
	{
		current_tile_x_left = (spr.getGlobalBounds().left - _game->getMap().getPosition().x) / (rh->tile_size);
		current_tile_x_right = ((spr.getGlobalBounds().left + spr.getGlobalBounds().width - 4) - _game->getMap().getPosition().x) / (rh->tile_size);
		current_tile_y_top = (spr.getGlobalBounds().top - _game->getMap().getPosition().y) / (rh->tile_size);
		current_tile_y_bottom = ((spr.getGlobalBounds().top + spr.getGlobalBounds().height) - _game->getMap().getPosition().y) / (rh->tile_size);
	}

	sf::IntRect Enemy::getFaceRect(sf::Vector2u size) {
		if (!flip)
			return sf::IntRect(size.x, 0, -1 * size.x, size.y);
		else
			return sf::IntRect(0, 0, size.x, size.y);
	}
}