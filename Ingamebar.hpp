#pragma once

#include "SFML/Graphics.hpp"
#include "ResourceHolder.hpp"

namespace rpf {
	class IngameBar {
	public:
		IngameBar(ResourceHolder* rh);
		sf::RectangleShape panel;
		sf::Sprite life;
		sf::Text life_text;
		sf::Text score;
		sf::Text highest_score;
		void update(int scores, int lifes);
	private:
		ResourceHolder* rh;
		float nowx;
	};

	class IngameMulBar {
	public:
		IngameMulBar(ResourceHolder* rh);
		sf::RectangleShape panel;
		sf::Sprite life;
		sf::Text time;
		void update(float time);
	private:
		ResourceHolder* rh;
		float nowx;
	};
}