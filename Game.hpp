#pragma once

#include "SFML/Graphics.hpp"
#include "RenderManager.hpp"
#include "ResourceHolder.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "Render.hpp"
#include "Bullet.hpp"
#include "Coin.hpp"
#include "Portal.hpp"
#include "Enemy.hpp"
#include "Ingamebar.hpp"
//#define RDEBUG

namespace rpf {
	class Game : public Render {
	public:
		Game();
		Game(RenderManager* rm, ResourceHolder* rh);
		~Game() = default;
		void update() override;
		void handleEvent(sf::Event e) override;
		virtual void player_shot(sf::FloatRect bounds, bool flip);
		virtual void resetLvl();
		virtual bool is_empty_block(int x, int y);
		virtual sf::Vector2f get_cord_of_tile(int x, int y);
		virtual Map getMap() { return map; }
	private:
		int idnum = 1;
		Player p;
		RenderManager* rm;
		ResourceHolder* rh;
		Map map;
		Map back_map;
		IngameBar bar;
		std::vector<int> current_level;
		std::vector<int> current_background;
		std::vector<Bullet*> bullets;
		std::vector<Coin*> coins;
		std::vector<Enemy*> enemies;
		int nowp = 0;
		std::vector<int> xs, ys;//add
		std::vector<int> toxs, toys;//add
		Portal* portal = nullptr;
		int level = 0;
		int px = -1, py = -1;
		void init_level();
		void init_bg();
		void init_render();
		void check_bullets_out();
		void check_bullets_hit_emy();
		void check_bullets_hit_block();
		void check_player_enemy();
		void check_coin();
		void check_portal();
		void check_enemy();
		void nextLvl();
		void setLvl(int lvl);
#ifdef RDEBUG
		int test = 0;
#endif
	};
}