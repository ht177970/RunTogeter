#include "Menu.hpp"
#include "Core.hpp"

#include <iostream>

namespace rpf {

#pragma region ClickableMenu

	ClickableMenu::ClickableMenu(RenderManager* _rm, ResourceHolder* _rh) {
		this->rm = _rm;
		this->rh = _rh;
		m_text_index = -1;
	}

	void ClickableMenu::handleEvent(sf::Event e) {
		switch (e.type) {
			case sf::Event::KeyPressed:
				onKeyDown(e.key.code);
				break;
			case sf::Event::MouseMoved:
				onMouseMove(e.mouseMove);
				break;
			case sf::Event::MouseButtonPressed:
				onMouseClick();
				break;
		}
	}

	void ClickableMenu::update() {
		for (Text* text : m_clickable_texts) {
			text->updateText();
			//text->grap.rotate(1);
		}
	}

	int ClickableMenu::changeTextIndex(int new_index) {
		this->m_text_index = new_index;
		return this->m_text_index;
	}

	void ClickableMenu::onKeyDown(sf::Keyboard::Key keycode) {
		switch (keycode) {
		case sf::Keyboard::Up:
			if (changeTextIndex(m_text_index - 1) < 0)
				changeTextIndex(this->getClickableSize() - 1);
			break;
		case sf::Keyboard::Down:
			if (changeTextIndex(m_text_index + 1) > this->getClickableSize() - 1)
				changeTextIndex(0);
			break;
		case sf::Keyboard::Enter:
			EnterPressed(m_text_index);
			break;
		default:
			otherKeyDown(keycode);
			break;
		}
	}

	void ClickableMenu::onMouseMove(sf::Event::MouseMoveEvent mouse) {
		if (rm->view) {
			mouse.x += rm->view->getCenter().x - rh->s_width / 2;
			mouse.y += rm->view->getCenter().y - rh->s_height / 2;
		}
		for (auto t : this->getBaseClickable()) {
			if (t->isPosIn(sf::Vector2i(mouse.x, mouse.y))) {
				changeTextIndex(t->getId());
				break;
			}
		}
	}

	void ClickableMenu::onMouseClick() {
		sf::Vector2i mousepos = sf::Mouse::getPosition(*rm->window);
		if (rm->view) {
			mousepos.x += rm->view->getCenter().x - rh->s_width / 2;
			mousepos.y += rm->view->getCenter().y - rh->s_height / 2;
		}
		if (this->getBaseClickable()[m_text_index]->isPosIn(mousepos))
			EnterPressed(m_text_index);
	}

#pragma endregion


#pragma region Text

	Text::Text(sf::String text, Pos position, sf::Font font) {
		this->m_id = -1;//default
		this->m_font = font;
		this->grap = sf::Text(text, m_font, 30U);//default
		this->grap.setOrigin(grap.getLocalBounds().width / 2, grap.getLocalBounds().height / 2);
		this->grap.setPosition(position.x, position.y);
		this->index = nullptr;
	}

	void Text::setId(int id) {
		this->m_id = id;
	}

	void Text::setTextIndexPointer(int* text_index) {
		this->index = text_index;
	}

	void Text::setTextSize(unsigned int size) {
		this->grap.setCharacterSize(size);
		this->grap.setOrigin(grap.getLocalBounds().width / 2, grap.getLocalBounds().height / 2);
		std::cout << grap.getLocalBounds().width << " " << grap.getLocalBounds().height << "\n";
		this->TextChanged();
	}

	void Text::setTextColor(sf::Color color) {
		this->grap.setFillColor(color);
	}

	int Text::getId() {
		return m_id;
	}

	void Text::draw(sf::RenderWindow& window) {
		updateText();
		window.draw(grap);
	}

	void Text::updateText() {
		if (this->index)
			this->setTextColor((*this->index == m_id) ? sf::Color::Yellow : sf::Color::White);
	}

	bool Text::isPosIn(sf::Vector2i pos) {
		sf::Vector2f grapPos = grap.getPosition();
		sf::FloatRect grap_rect = grap.getLocalBounds();
		pos.x -= grapPos.x + grap_rect.left - grap_rect.width / 2 ;
		pos.y -= grapPos.y + grap_rect.top - grap_rect.height / 2;
		return 0 <= pos.x && pos.x <= grap_rect.width &&
			0 <= pos.y && pos.y <= grap_rect.height;
	}

#pragma endregion

	MainMenu::MainMenu(RenderManager* rm, ResourceHolder* rh) : ClickableMenu(rm, rh) {
		this->rm = rm;
		this->rh = rh;
		this->initMenu();
	}

	void MainMenu::initMenu() {
		sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(rh->s_width, rh->s_height));
		rect->setFillColor(sf::Color::Cyan);
		rect->setOrigin(rh->s_width / 2, 0);
		rect->setPosition(rh->view->getCenter().x, 0);
		rm->addGraphics(rect);

		sf::String texts[] = { "Play with others", "Quit the game" };
		Pos position = Pos(rh->s_width / 2, rh->s_height / 4);
		unsigned int text_size;
		text_size = static_cast<unsigned int>(30.f * position.x / 500);
		for (int i = 0; i < 2; i++) {
			Text* tmp = new Text(texts[i], position.AddY(text_size * 2), rh->font);
			tmp->setTextSize(30U);
			tmp->setId(i);
			tmp->setTextIndexPointer(&m_text_index);
			m_clickable_texts.push_back(tmp);
		}
		for (Text* t : m_clickable_texts)
			rm->addGraphics(&t->grap);
		m_text_index = 0;
	}

	void MainMenu::EnterPressed(int index) {
		if (index == 0)
			Core::CORE->switchMode(Mode::IN_GAME);
		else if (index == 1)
			Core::CORE->switchMode(Mode::CLOSED);
	}

	GameOverMenu::GameOverMenu(RenderManager* rm, ResourceHolder* rh) : ClickableMenu(rm, rh) {
		this->rm = rm;
		this->rh = rh;
		this->initMenu();
	}

	void GameOverMenu::EnterPressed(int index) {
		if (index == 0)
			Core::CORE->switchMode(Mode::IN_GAME);
		else if (index == 1)
			Core::CORE->switchMode(Mode::CLOSED);
	}

	void GameOverMenu::initMenu() {
		sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(rh->s_width, rh->s_height));
		rect->setFillColor(sf::Color(0x00000088));
		rect->setOrigin(rh->s_width / 2, 0);
		rect->setPosition(rh->view->getCenter().x, 0);
		rm->addGraphics(rect);

		sf::String texts[] = { "START"/*, L"回到選單"*/, "LEAVE" };
		Pos text_position(rh->view->getCenter().x, rh->view->getCenter().y- rh->s_height / 4);
		unsigned int text_size;
		text_size = static_cast<unsigned int>(30.f * text_position.x / 500);
		for (int i = 0; i < 2; i++) {
			Text* tmp = new Text(texts[i], text_position.AddY(text_size*2), rh->font);
			tmp->setTextSize(30U);
			tmp->setId(i);
			tmp->setTextIndexPointer(&m_text_index);
			m_clickable_texts.push_back(tmp);
		}
		for (Text* t : m_clickable_texts)
			rm->addGraphics(&t->grap);
		m_text_index = 0;
	}
}