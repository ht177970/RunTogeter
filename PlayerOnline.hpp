#pragma once

#include "SFML/Graphics.hpp"
#include "ResourceHolder.hpp"
#include "Map.hpp"

namespace rpf {
	class PlayerOnline {
	public:
		int score = 0;
		PlayerOnline(ResourceHolder* rh, int id);
		void update();
		void dead();
		bool isDead() { return killed; }
		int getId() { return id; }
		sf::Sprite& getDrawable() { return this->current_sprite; }
		bool shooting = 0, killed = 0;
		float x_speed = 0, y_speed = 0;
		int anim_index = 0;
	private:
		int id;
		const float unit_speed = 7;
		const int ani_speed = 2;
		const int dead_delay = 40;
		const float g = 0.8;
		bool w_key = false;
		bool space_key = false;
		bool left_key = false;
		bool right_key = false;
		bool flip = false;
		int delay = 0;
		ResourceHolder* rh;
		sf::Sprite current_sprite;
		void check_face();
		void death_anim();
		void update_spr();
		bool onGround();
		int fixRunningHeight(int height);
		sf::IntRect getFaceRect(sf::Vector2u size);
	};
}