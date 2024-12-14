#pragma once

#include "SFML/Graphics.hpp"
#include "ResourceHolder.hpp"
#include "Game.hpp"
#include "RenderManager.hpp"
#include "Menu.hpp"

namespace rpf {
	enum Mode {
		MAIN_MENU, CONNECTION_MENU, IN_GAME, GAME_OVER, CLOSED
	};

	class Core {
	public:
		static Core* CORE;
		static int highest_score;
		Core();
		~Core() = default;
		void Run();
		void switchMode(Mode mode);
	private:
		sf::RenderWindow window;
		RenderManager rm;
		ResourceHolder rh;
		Render* now;
		Mode mode;
		void update();
	};
}