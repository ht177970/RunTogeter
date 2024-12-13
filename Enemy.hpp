#pragma once

#include "SFML/Graphics.hpp"
#include "ResourceHolder.hpp"

namespace rpf {
	class Game;
	class Enemy {
	public:
		Enemy(ResourceHolder* rh, Game* g, int x, int y, int id);
		~Enemy() = default;
		bool AI = false;
		bool killed = false;
		void update();
		void dead();
		bool isDead() { return killed && anim_index >= 30; }
		int getScore() { return AI ? 50 : 20; }
		int getId() { return id; }
		sf::Sprite& getDrawable() { return spr; }
	private:
		int id;
		const int anim_speed = 5;
		int anim_index = 0;
		int ix, iy;
		ResourceHolder* rh;
		sf::Sprite spr;

		const int unit_speed = 2;
		Game* _game;
		bool flip = false;
		int x_speed = 0;
		int current_tile_x_left = 0;
		int current_tile_x_right = 0;
		int current_tile_y_top = 0;
		int current_tile_y_bottom = 0;
		void check_move_x();
		void update_current_tiles();
		sf::IntRect getFaceRect(sf::Vector2u size);
	};
}