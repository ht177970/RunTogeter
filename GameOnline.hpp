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
#include "PlayerOnline.hpp"
#include "Game.hpp"
#include <unordered_map>
#include <mutex>

struct data {
    float x = 0, y = 0;
    float x_speed = 0, y_speed = 0;

    bool operator!=(const data& ot) {
        return x != ot.x || y != ot.y || x_speed != ot.x_speed || y_speed  != ot.y_speed;
    }
};

namespace rpf {
	class GameOnline : public Game {
	public:
		GameOnline(RenderManager* rm, ResourceHolder* rh);
		~GameOnline() = default;
		void update() override;
		void handleEvent(sf::Event e) override;
		void player_shot(sf::FloatRect bounds, bool flip) override;
		void resetLvl() override;
		void resetLvlAsync() { wait = true; };
		bool is_empty_block(int x, int y) override;
		sf::Vector2f get_cord_of_tile(int x, int y) override;
		Map getMap() override { return map; }
		std::mutex obj_mutex;
		std::vector<Bullet*> bullets_out, bullets_in;
		std::unordered_map<int, data> poses;
		std::unordered_map<int, int> shoots, deads;
		data player_data;
		bool shoot = false;
		bool dead = false;

		ResourceHolder* rh;//
	private:
		int idnum = 1;
		Player p;
		RenderManager* rm;
		Map map;
		Map back_map;
		IngameBar bar;
		std::vector<int> current_level;
		std::vector<int> current_background;
		std::vector<Bullet*> bullets;
		std::vector<Coin*> coins;
		std::vector<Enemy*> enemies;
		std::unordered_map<int, PlayerOnline*> online_players;
		int nowp = 0;
		std::vector<int> xs, ys;//add
		std::vector<int> toxs, toys;//add
		sf::Clock clk;
		Portal* portal = nullptr;
		int level = 0;
		int px = -1, py = -1;
		bool finish = false;
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
		bool wait = false;
		bool init = false;
#ifdef RDEBUG
		int test = 0;
#endif
	};
}