#include <cstring>
#include "ResourceHolder.hpp"

namespace rpf {
	ResourceHolder::ResourceHolder() {
		font.loadFromFile("font.ttf");
		loadMaps();
		loadTextures();
		loadSounds();
	}

	void ResourceHolder::loadMaps() {
		for (int i = 0; i < no_of_maps; i++)
		{
			sf::Image map;
			map.loadFromFile("img/levels/" + std::to_string(i) + ".png");
			maps.push_back(map);
		}

		back_tex.loadFromFile("img/sky.png");
		back_sprite.setTexture(back_tex);
		back_sprite.setScale(2, 1);
		map_back.loadFromFile("img/map1_back.png");

		view = new sf::View(sf::FloatRect(0, 0, s_width, s_height));
		view->setCenter((maps[0].getSize().x * tile_size) / 2, s_height / 2);
	}

	void ResourceHolder::loadTextures() {
		jump_gun.loadFromFile("img/_Mode-Gun/05-Jump/JK_P_Gun__Jump_000.png");
		bullet.loadFromFile("img/Bullet.png");
		enemy_die.loadFromFile("img/monster/got_hit/frame.png");
		for (int i = 0; i <= 9; i++)
		{
			sf::Texture rg;
			sf::Texture pi;

			sf::Texture die;
			sf::Texture shoot;

			rg.loadFromFile("img/_Mode-Gun/02-Run/JK_P_Gun__Run_00" + std::to_string(i) + ".png");
			pi.loadFromFile("img/_Mode-Gun/01-Idle/JK_P_Gun__Idle_00" + std::to_string(i) + ".png");
			die.loadFromFile("img/_Mode-Gun/06-Die/JK_P__Die_00" + std::to_string(i) + ".png");
			shoot.loadFromFile("img/_Mode-Gun/03-Shot/JK_P_Gun__Attack_00" + std::to_string(i) + ".png");
			if (i + 1 <= 6) {
				sf::Texture c;
				c.loadFromFile("img/coin/Coin_0" + std::to_string(i + 1) + ".png");
				coin.push_back(c);
			}
			if (i + 1 <= 2) {
				sf::Texture e;
				e.loadFromFile("img/monster/idle/frame-" + std::to_string(i + 1) + ".png");
				enemy.push_back(e);
			}

			gun_run.push_back(rg);
			player_idle.push_back(pi);
			player_die.push_back(die);
			shooting.push_back(shoot);
		}
	}

	void ResourceHolder::loadSounds() {
		music.openFromFile("sound/music.ogg");
		music.setLoop(true);
		music.setVolume(50);

		s_jump.loadFromFile("sound/jump.wav");
		jump.setBuffer(s_jump);
		s_shot.loadFromFile("sound/shot.wav");
		shot.setBuffer(s_shot);
		shot.setVolume(30);
		s_death.loadFromFile("sound/death.wav");
		death.setBuffer(s_death);
		s_fall.loadFromFile("sound/fall.wav");
		fall.setBuffer(s_fall);
		s_pickup.loadFromFile("sound/pickup.wav");
		pickup.setBuffer(s_pickup);
		pickup.setVolume(70);
		s_gameover.loadFromFile("sound/gameover.wav");
		gameover.setBuffer(s_gameover);
		s_monster_death.loadFromFile("sound/monster_death.wav");
		monster_death.setBuffer(s_monster_death);
		s_cheer.loadFromFile("sound/cheer.wav");
		cheer.setBuffer(s_cheer);
	}
}