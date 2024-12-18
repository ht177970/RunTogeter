#include "Core.hpp"
#pragma comment (lib ,"imm32.lib")

namespace rpf {

	Core* Core::CORE = nullptr;
	GameOnline* Core::GAME = nullptr;
	int Core::highest_score = 0;

	Core::Core() : mode(Mode::MAIN_MENU), rm(&window) {//mode(Mode::MAIN_MENU)
		window.create(sf::VideoMode(rh.s_width, rh.s_height), "Ryan PlatFormer v3.0", sf::Style::Resize | sf::Style::Close);
		window.setFramerateLimit(60);
		window.setKeyRepeatEnabled(false);
		now = nullptr;
		rh.music.play();
		Core::CORE = this;
	}
	
	void Core::Run() {
		switchMode(Mode::MAIN_MENU);//start with ...
		while (window.isOpen())
			this->update();
	}

	void Core::update() {
		if (wait_switch) {
			wait_switch = false;
			this->switchMode(wait_mode);
		}
		sf::Event e;
		while (window.pollEvent(e))
			if (e.type == sf::Event::Closed)
				window.close();
			else if (e.type == sf::Event::Resized) {
				float screenWidth = 160.f;
				float screenHeight = 96.f;
				sf::Vector2u size = window.getSize();
				float heightRatio = screenHeight / screenWidth;
				float widthRatio = screenWidth / screenHeight;
				if (size.y * widthRatio <= size.x)
				{
					size.x = size.y * widthRatio;
				}
				else if (size.x * heightRatio <= size.y)
				{
					size.y = size.x * heightRatio;
				}
				rh.s_width = size.x;
				rh.s_height = size.y;
				window.setSize(size);
			}
			else
				now->handleEvent(e);
		now->update();
		rm.Render();
	}

	void Core::switchMode(Mode mode) {
		switch (mode) {
		case Mode::MAIN_MENU:
			if (now)
				free(now);
			rm.clear();
			now = new MainMenu(&rm, &rh);
			break;
		case Mode::CONNECTION_MENU:
			if (now)
				free(now);
			rm.clear();
			now = new ConnectionMenu(&rm, &rh);
			break;
		//case Mode::IN_GAME:
		case Mode::SINGLE_GAME:
			if (now)
				free(now);
			rm.clear();
			now = new Game(&rm, &rh);
			break;
		case Mode::MULTI_GAME:
			if (now)
				free(now);
			rm.clear();
			now = Core::GAME = new GameOnline(&rm, &rh);
			break;
		case Mode::GAME_OVER:
			now = new GameOverMenu(&rm, &rh);
			break;
		case Mode::CLOSED:
			window.close();
			break;
		}
	}

	void Core::switchMode(Render* obj) {
		if (!obj)
			throw std::runtime_error("fatal error: obj shouldn't be NULL!");
		if (now)
			free(now);
		now = obj;
	}

	void Core::switchModeAsync(Mode mode) {
		wait_switch = true;
		wait_mode = mode;
	}

	
}