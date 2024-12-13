#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

namespace rpf {
	class ResourceHolder {
	public:

		int s_width = 1600;
		int s_height = 960;
		const int no_of_maps = 1;

		float trans = 0.75;
		int tile_size = trans * 64;

		sf::View* view;

		sf::Texture back_tex;
		sf::Sprite back_sprite;

		sf::Texture bullet;
		sf::Texture jump_gun;
		sf::Texture enemy_die;

		std::vector<sf::Texture> gun_run;
		std::vector<sf::Texture> player_idle;
		std::vector<sf::Texture> shooting;
		std::vector<sf::Texture> player_die;
		std::vector<sf::Texture> coin;
		std::vector<sf::Texture> enemy;

		std::vector<sf::Image> maps;
		sf::Image map_back;

		sf::Music music;
		sf::Sound jump;
		sf::Sound shot;
		sf::Sound death;
		sf::Sound fall;
		sf::Sound pickup;
		sf::Sound gameover;
		sf::Sound monster_death;
		sf::Sound cheer;

		sf::Font font;

		ResourceHolder();
	private:
		sf::SoundBuffer s_jump;
		sf::SoundBuffer s_shot;
		sf::SoundBuffer s_death;
		sf::SoundBuffer s_fall;
		sf::SoundBuffer s_pickup;
		sf::SoundBuffer s_gameover;
		sf::SoundBuffer s_monster_death;
		sf::SoundBuffer s_cheer;
		void loadMaps();
		void loadTextures();
		void loadSounds();
	};
}