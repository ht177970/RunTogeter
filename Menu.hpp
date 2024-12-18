#pragma once

#include <SFML/Graphics.hpp>
#include "Render.hpp"
#include "RenderManager.hpp"
#include "ResourceHolder.hpp"

namespace rpf {

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

	class Text {
	public:
		explicit Text(sf::String text, Pos position, sf::Font font);
		~Text() = default;
		void setId(int id);
		void setTextIndexPointer(int* text_index);
		void setTextSize(unsigned int size);
		void setTextColor(sf::Color color);
		int getId();
		virtual void TextChanged() {};
		virtual void updateText();
		virtual bool isPosIn(sf::Vector2i pos);
		sf::Text& getDrawable() { return grap; }
	protected:
		RenderManager* rm;
		ResourceHolder* rh;
		sf::Text grap;
		sf::Font m_font;
		int m_id;
		int* index;
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
		void update() override;
	};

	class GameOverMenu : public ClickableMenu {
	public:
		explicit GameOverMenu(RenderManager* rm, ResourceHolder* rh);
		~GameOverMenu() = default;
		void initMenu();
		void EnterPressed(int index);
	};

	class GameOverMulMenu : public ClickableMenu {
	public:
		explicit GameOverMulMenu(RenderManager* rm, ResourceHolder* rh);
		~GameOverMulMenu() = default;
		void initMenu();
		void EnterPressed(int index);
	};

	class TextBox {
	public:
		TextBox(Pos position, sf::Font font, unsigned int size, RenderManager* rm);

		void handleEvent(sf::Event e);
		void setPlaceholder(const sf::String& text);
		sf::String getInputText() const;

		sf::RectangleShape* getBox();
		sf::Text* getInputTextGraphic();
		sf::Text* getPlaceholderTextGraphic();

	private:
		RenderManager* rm;
		sf::RectangleShape box;
		sf::Text inputText;
		sf::Text placeholderText;
		sf::Font m_font;
		bool isFocused;
		void updateVisuals();
	};


	class ConnectionMenu : public ClickableMenu {
	public:
		explicit ConnectionMenu(RenderManager* rm, ResourceHolder* rh);
		~ConnectionMenu() = default;
		void initMenu();
		void EnterPressed(int index) override;
		void handleEvent(sf::Event e) override;
		void update() override;
	private:
		TextBox* textBox;
		sf::Text errorText;
	};

	class RoomMenu : public ClickableMenu {
	public:
		explicit RoomMenu(RenderManager* rm, ResourceHolder* rh, std::vector<int> players, int selfId);

		void initMenu();
		void EnterPressed(int index) override;
		void update() override;
		std::vector<int> joined, leaved;
		int selfId;
		bool roomOwner = false;
	private:
		std::vector<int> players;
		std::vector<std::pair<int, sf::Text*>> playerTexts;
		sf::Font m_font;

		void renderPlayers();
	};
}