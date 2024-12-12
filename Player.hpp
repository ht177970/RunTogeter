#pragma once

#include "SFML/Graphics.hpp"
#include "ResourceHolder.hpp"
#include "Map.hpp"

namespace rpf {
	class Game;
	class Player {
	public:
		int score = 0;
		Player(ResourceHolder *rh, Map* m, Game* _game);
		void KeyPress(sf::Keyboard::Key k);
		void KeyRelease(sf::Keyboard::Key k);
		void update();
		void spawn(int x, int y);
		void dead();
		bool isDead() { return killed; }
		int getLife() { return life; }
		sf::Sprite& getDrawable() { return this->current_sprite; }
	private:
		int life = 3;
		const float unit_speed = 7;//10
		const int ani_speed = 3;
		const int dead_delay = 60;
		//const int shot_dx = 15;
		const float g = 0.8;
		bool w_key = false;
		bool space_key = false;
		bool left_key = false;
		bool right_key = false;
		bool flip = false;
		bool shooting = 0, killed = 0;
		float x_speed = 0, y_speed = 0;
		int anim_index = 0;
		int delay = 0;
		int current_tile_x_left = 0;
		int current_tile_x_right = 0;
		int current_tile_y_top = 0;
		int current_tile_y_bottom = 0;
		ResourceHolder* rh;
		Map* back_map;
		Game* _game;
		sf::Sprite current_sprite;
		void jump();
		void shoot();
		void move_map();
		void check_face();
		void check_move_x();
		void check_move_y();
		void check_out();
		void death_anim();
		void update_spr();
		void update_current_tiles();
		bool onGround();
		int fixRunningHeight(int height);
		sf::IntRect getFaceRect(sf::Vector2u size);
	};
}