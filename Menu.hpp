#pragma once

#include <SFML/Graphics.hpp>
#include "Render.hpp"
#include "RenderManager.hpp"
#include "ResourceHolder.hpp"

namespace rpf {
	class BaseDrawable {
	public:
		virtual ~BaseDrawable() = default;
		virtual void draw(sf::RenderWindow& window) = 0;
	};

	struct Pos {
		float x, y;

		Pos(int x, int y) {
			this->x = x;
			this->y = y;
		}

		Pos(unsigned int x, unsigned int y) {
			this->x = x;
			this->y = y;
		}

		Pos(float x, float y) {
			this->x = x;
			this->y = y;
		}

		Pos(sf::Vector2f v2f) {
			this->x = v2f.x;
			this->y = v2f.y;
		}

		Pos(sf::Vector2i v2i) {
			this->x = v2i.x;
			this->y = v2i.y;
		}

		Pos AddX(float delta) {
			this->x += delta;
			return *this;
		}

		Pos AddY(float delta) {
			this->y += delta;
			return *this;
		}

		bool operator==(Pos other) {
			return (x == other.x && y == other.y);
		}
	};

	class Text : public BaseDrawable {
	public:
		explicit Text(sf::String text, Pos position, sf::Font font);
		~Text() = default;
		void setId(int id);
		void setTextIndexPointer(int* text_index);
		void setTextSize(unsigned int size);
		void setTextColor(sf::Color color);
		virtual void TextChanged() {};
		int getId();
		void draw(sf::RenderWindow& window) override;
		virtual bool isPosIn(sf::Vector2i pos);
	//protected:
		RenderManager* rm;
		ResourceHolder* rh;
		sf::Text grap;
		sf::Font m_font;
		int m_id;
		int* index;
		virtual void updateText();
	};

	class ClickableMenu : public Render {
	public:
		void update() override;
		void handleEvent(sf::Event e) override;
		virtual int getClickableSize() { return m_clickable_texts.size(); };
		virtual std::vector<Text*> getBaseClickable() { return m_clickable_texts; };
	protected:
		ClickableMenu(RenderManager* _rm, ResourceHolder* _rh);
		RenderManager* rm;
		ResourceHolder* rh;
		int m_text_index;
		std::vector<Text*> m_clickable_texts;
		int changeTextIndex(int new_index);
		void onKeyDown(sf::Keyboard::Key keycode);
		void onMouseMove(sf::Event::MouseMoveEvent mouse);
		void onMouseClick();
		virtual void otherKeyDown(sf::Keyboard::Key keycode) {};
		virtual void EnterPressed(int index) = 0;
	};

	class MainMenu : public ClickableMenu {
	public:
		explicit MainMenu(RenderManager* rm, ResourceHolder* rh);
		~MainMenu() = default;
		void initMenu();
		void EnterPressed(int index);
	};

	class GameOverMenu : public ClickableMenu {
	public:
		explicit GameOverMenu(RenderManager* rm, ResourceHolder* rh);
		~GameOverMenu() = default;
		void initMenu();
		void EnterPressed(int index);
	};
}