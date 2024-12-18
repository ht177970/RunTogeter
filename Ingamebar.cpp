#include "Ingamebar.hpp"
#include <string>
#include "Core.hpp"

namespace rpf {
	IngameBar::IngameBar(ResourceHolder* rh) {
		if (!rh)
			return;
		this->rh = rh;
		panel.setSize(sf::Vector2f(rh->s_width, 100));
		panel.setFillColor(sf::Color(0x00000055));

		life.setTexture(rh->player_idle[0], true);
		life.setScale(0.2, 0.2);
		life.setPosition(10, 10);

		life_text.setFont(rh->font);
		life_text.setString("x3");
		life_text.setOrigin(0, life_text.getLocalBounds().height * 2);
		life_text.setPosition(120, 100);

		score.setFont(rh->font);
		score.setString("score: 0");
		score.setCharacterSize(60U);
		score.setOrigin(0, score.getLocalBounds().height / 2);
		score.setPosition(600, 50);

		highest_score.setFont(rh->font);
		highest_score.setString("history: 0");
		highest_score.setCharacterSize(60U);
		highest_score.setOrigin(0, score.getLocalBounds().height / 2);
		highest_score.setPosition(1000, 50);

		nowx = rh->s_width / 2;
	}

	void IngameBar::update(int scores, int lifes) {
		panel.move(rh->view->getCenter().x - nowx, 0);
		life.move(rh->view->getCenter().x - nowx, 0);
		life_text.move(rh->view->getCenter().x - nowx, 0);
		score.move(rh->view->getCenter().x - nowx, 0);
		highest_score.move(rh->view->getCenter().x - nowx, 0);

		life_text.setString("x" + std::to_string(lifes));
		score.setString("score: " + std::to_string(scores));
		highest_score.setString("history: " + std::to_string(Core::highest_score));

		nowx = rh->view->getCenter().x;
	}
	IngameMulBar::IngameMulBar(ResourceHolder* rh) {
		if (!rh)
			return;
		this->rh = rh;
		panel.setSize(sf::Vector2f(rh->s_width, 100));
		panel.setFillColor(sf::Color(0x00000055));

		life.setTexture(rh->player_idle[0], true);
		life.setScale(0.2, 0.2);
		life.setPosition(10, 10);

		time_text.setFont(rh->font);
		time_text.setString("0.000000s");
		time_text.setOrigin(0, time_text.getLocalBounds().height * 2);
		time_text.setPosition(120, 100);

		nowx = rh->s_width / 2;
	}

	void IngameMulBar::update(float time) {
		panel.move(rh->view->getCenter().x - nowx, 0);
		life.move(rh->view->getCenter().x - nowx, 0);
		time_text.move(rh->view->getCenter().x - nowx, 0);

		time_text.setString(std::to_string(time) + "s");

		nowx = rh->view->getCenter().x;
	}
}