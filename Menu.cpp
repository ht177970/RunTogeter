#include "Menu.hpp"
#include "Core.hpp"

namespace rpf {
	GameOverMenu::GameOverMenu(RenderManager* rm, ResourceHolder* rh) {
		this->rm = rm;
		this->rh = rh;
		this->init();
	}

	void GameOverMenu::update() {
		for (int i = 0; i < clickable_texts.size(); i++) {
			if (m_text_index == i)
				clickable_texts[i]->setFillColor(sf::Color::Yellow);
			else
				clickable_texts[i]->setFillColor(sf::Color::White);
		}
	}

	void GameOverMenu::handleEvent(sf::Event e) {
		if (e.type == sf::Event::KeyPressed) {
			if (e.key.code == sf::Keyboard::Up) {
				m_text_index--;
				if (m_text_index < 0)
					m_text_index = clickable_texts.size() - 1;
			}
			else if (e.key.code == sf::Keyboard::Down) {
				m_text_index++;
				if (m_text_index == clickable_texts.size())
					m_text_index = 0;
			}
			else if (e.key.code == sf::Keyboard::Enter) {
				if (m_text_index == 0)
					Core::CORE->switchMode(Mode::IN_GAME);
				else if (m_text_index == 1)
					Core::CORE->switchMode(Mode::CLOSED);
			}
		}
	}

	void GameOverMenu::init() {
		sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(rh->s_width, rh->s_height));
		rect->setFillColor(sf::Color(0x00000088));
		rect->setOrigin(rh->s_width / 2, 0);
		rect->setPosition(rh->view->getCenter().x, 0);
		rm->addGraphics(rect);

		sf::String texts[] = { "START"/*, L"回到選單"*/, "LEAVE" };
		int x = rh->view->getCenter().x;
		int y = 500;//400
		int next_y = 120;

		for (int i = 0; i < 2; i++) {
			sf::Text* tmp = new sf::Text(texts[i], rh->font, 60U);
			tmp->setOrigin(tmp->getLocalBounds().width / 2, tmp->getLocalBounds().height / 2);
			tmp->setPosition(x, y);
			y += next_y;
			clickable_texts.push_back(tmp);
		}
		for (sf::Text* t : clickable_texts)
			rm->addGraphics(t);
		m_text_index = 0;
	}
}