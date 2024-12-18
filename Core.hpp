#pragma once

#include "SFML/Graphics.hpp"
#include "ResourceHolder.hpp"
#include "Game.hpp"
#include "GameOnline.hpp"
#include "RenderManager.hpp"
#include "Menu.hpp"
//#include "MySocket.hpp"
#include "SocketManager.hpp"

namespace rpf {
	enum Mode {
		MAIN_MENU, CONNECTION_MENU, SINGLE_GAME, MULTI_GAME, GAME_OVER, GAME_OVER_MUL, CLOSED
	};

	class Core {
	public:
		static Core* CORE;
		static GameOnline* GAME;
		static SocketManager* sock;
		static int highest_score;
		Core();
		~Core() = default;
		void Run();
		void switchMode(Mode mode);
		void switchMode(Render* obj);
		void switchModeAsync(Mode mode);
		std::vector<int> joined, leaved;
		std::vector<std::pair<int, float>> results;
	private:
		sf::RenderWindow window;
		RenderManager rm;
		ResourceHolder rh;
		Render* now;
		Mode mode;
		bool wait_switch = false;
		Mode wait_mode;
		void update();
	};
}