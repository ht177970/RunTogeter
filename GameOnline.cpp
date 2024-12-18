#include "GameOnline.hpp"
#include <iostream>
#include "Bullet.hpp"
#include "Core.hpp"
#include "PlayerOnline.hpp"
#include "MySocket.cpp"

namespace rpf {

	void GameOnline::init_level() {
		int id = 0;
		current_level.clear();
		bool f = 0;
		for (int i = 0; i < rh->maps[level].getSize().y; i++)
		{
			for (int j = 0; j < rh->maps[level].getSize().x; j++)
			{
				unsigned int c = rh->maps[level].getPixel(j, i).toInteger();
				id = -1;
				switch (c) {
				case 0x4CFF00FF:
					id = 1;//10
					break;
				case 3365030143://4
				case 3111802879://5
				case 3351529983://6
					id = 0;//5
					break;
				case 4294050047:
					p.spawn(j, i);
					break;
				case 4291366655://coin
				{
					/*Coin* coin = new Coin(rh, j, i, idnum++);
					coins.push_back(coin);
					rm->addGraphics(&coin->getDrawable());*/
					break;
				}
				case 3978044671:
				case 2281707007:
				{
					Enemy* emy = new Enemy(rh, this, j, i, idnum++);
					if (c == -2013260289)
						emy->AI = true;
					enemies.push_back(emy);
					rm->addGraphics(&emy->getDrawable());
					break;
				}
				}

				current_level.push_back(id);
			}
		}
		for (int j = 0; j < rh->maps[level].getSize().x; j++)
		{
			for (int i = 0; i < rh->maps[level].getSize().y; i++)
			{
				unsigned int c = rh->maps[level].getPixel(j, i).toInteger();
				id = -1;
				switch (c) {
				case 10676479://end(portal)
					if (f == 0) {
						this->portal = new Portal(rh, j, i);
						rm->addGraphics(&portal->getDrawable());
						//px = j, py = i;
						xs.push_back(j);
						ys.push_back(i);
					}
					else {
						toxs.push_back(j);
						toys.push_back(i);
					}
					f = !f;
					break;

				}

				current_level.push_back(id);
			}
		}
		nowp = 0;
		px = xs[nowp];
		py = ys[nowp];
		//sf::Color col = rh->maps[level].getPixel(6, 14);//
		//std::cout << col.toInteger() << '\n';//
		map.load("img/Tiles.png", sf::Vector2u(64, 64), current_level, rh->maps[level].getSize().x, rh->maps[level].getSize().y);
		map.setScale(rh->trans, rh->trans);
	}

	void GameOnline::init_bg() {
		int id = -1;
		current_background.clear();

		for (int i = 0; i < rh->map_back.getSize().y; i++)
		{
			for (int j = 0; j < rh->map_back.getSize().x; j++)
			{
				sf::Color c = rh->map_back.getPixel(j, i);

				if (c == sf::Color(0x7F0037FF))
				{
					id = 2;//37;
				}
				else if (c == sf::Color(0xFF006EFF))
				{
					id = 3;//38;
				}
				else { id = -1; }

				current_background.push_back(id);
			}
		}
		back_map.load("img/Tiles.png", sf::Vector2u(64, 64), current_background, rh->map_back.getSize().x, rh->map_back.getSize().y);
		back_map.setPosition(-1000, 0);
		back_map.setScale(rh->trans, rh->trans);
	}

	void GameOnline::init_render() {
		rh->view->setCenter(rh->s_width * 4 / 3 - 100, rh->s_height / 2);
		rm->setView(rh->view);
		rm->addGraphics(&rh->back_sprite);
		rm->addGraphics(&back_map);
		rm->addGraphics(&map);
		for (auto& it : poses) {
			int id = it.first;
			PlayerOnline* online_player = new PlayerOnline(rh, id);
			online_players[id] = online_player;
			rm->addGraphics(&online_player->getDrawable());
		}
		rm->addGraphics(&p.getDrawable());

		rm->addGraphics(&bar.panel);
		rm->addGraphics(&bar.life);
		rm->addGraphics(&bar.life_text);
		rm->addGraphics(&bar.score);
		rm->addGraphics(&bar.highest_score);
	}

	GameOnline::GameOnline(RenderManager* _rm, ResourceHolder* _rh) : p(_rh, &back_map, this, idnum++), bar(_rh) {
		this->rm = _rm;
		this->rh = _rh;
		//this->setLvl(0);
	}

	void GameOnline::handleEvent(sf::Event e) {
		if (e.type == sf::Event::KeyPressed)
			p.KeyPress(e.key.code);
		else if (e.type == sf::Event::KeyReleased)
			p.KeyRelease(e.key.code);
	}

	void GameOnline::update() {
		if (!init) {
			std::lock_guard<std::mutex> lock(obj_mutex);
			if (wait) {
				wait = false;
				this->setLvl(level);
			}
			return;
		}
		if (p.score > Core::highest_score)
			Core::highest_score = p.score;

		if (p.getLife() == 0) {
			rh->gameover.play();
			Core::CORE->switchMode(rpf::Mode::GAME_OVER);
			return;
		}

		{
			std::lock_guard<std::mutex> lock(obj_mutex);
			for (Bullet* b : bullets_in) {
				bullets.push_back(b);
				rm->addGraphics(b);
			}
			for (auto& [id, data] : poses) {
				if (online_players.find(id) == online_players.end())
					continue;
				PlayerOnline* op = online_players[id];
				op->getDrawable().setPosition(data.x, data.y);
				op->x_speed = data.x_speed;
				op->y_speed = data.y_speed;
			}
			for (auto& [_, id] : shoots) {
				if (online_players.find(id) == online_players.end())
					continue;
				PlayerOnline* op = online_players[id];
				op->shooting = 1;
				op->anim_index = 0;
			}
			for (auto& [id, data] : deads) {
				if (online_players.find(id) == online_players.end())
					continue;
				PlayerOnline* op = online_players[id];
				op->killed = 1;
				op->anim_index = 0;
			}
			bullets_in.clear();
			shoots.clear();
			deads.clear();
		}

		for (auto& [_, op] : online_players)
			op->update();
		p.update();
		for (Bullet* b : bullets)
			b->update();
		for (Coin* c : coins)
			c->update();
		for (Enemy* emy : enemies)
			emy->update();
		if (portal)
			portal->update();
		bar.update(p.score, p.getLife());

		if (!p.isDead()) {
			this->check_bullets_hit_emy();
			this->check_bullets_hit_block();
			this->check_player_enemy();
			this->check_coin();
			this->check_portal();
			this->check_enemy();
		}

		{
			std::lock_guard<std::mutex> lock(obj_mutex);
			sf::Vector2f pos = p.getDrawable().getPosition();
			player_data.x = pos.x;
			player_data.y = pos.y;
			player_data.x_speed = p.getXspeed();
			player_data.y_speed = p.getYspeed();
			if (p.getShoot() && p.getAnim() == 1)
				shoot = 1;
			if (p.getKill() && p.getAnim() == 1)
				dead = 1;
		}

		rh->back_sprite.setPosition(rh->view->getCenter().x - rh->view->getSize().x, 0);
	}

	bool GameOnline::is_empty_block(int x, int y) {
		if (x < 0 || x >= map.m_width || y < 0 || y >= map.m_height)
			return true;
		return current_level[y * map.m_width + x] == -1;
	}

	sf::Vector2f GameOnline::get_cord_of_tile(int x, int y)
	{
		return sf::Vector2f((x * rh->tile_size) + map.getPosition().x, y * rh->tile_size + map.getPosition().y);
	}

	void GameOnline::player_shot(sf::FloatRect bounds, bool flip) {
		Bullet* b = new Bullet(bounds.left + (flip ? 0 : bounds.width - 15), bounds.top + 35,
			flip ? -20 : 20, rh);
		{
			std::lock_guard<std::mutex> lock(obj_mutex);
			bullets_out.push_back(b);
		}
		bullets.push_back(b);
		rm->addGraphics(b);
	}

	void GameOnline::check_bullets_out() {
		for (auto b = bullets.begin(); b != bullets.end();) {
			auto pos = rm->getPosOnWindow(*b);
			if (pos.x < 0 || pos.x > rh->s_width) {
				rm->delGraphic(*b);
				b = bullets.erase(b);
			}
			else
				b++;
		}
	}

	void GameOnline::check_bullets_hit_emy() {
		bool f;
		for (auto b = bullets.begin(); b != bullets.end();) {
			f = false;
			for (auto emy = enemies.begin(); emy != enemies.end(); emy++) {
				if (!(*emy)->killed && (*b)->getGlobalBounds().intersects((*emy)->getDrawable().getGlobalBounds()))
				{
					p.score += (*emy)->getScore() * (level + 1);
					rm->delGraphic(*b);
					b = bullets.erase(b);
					rh->monster_death.play();
					(*emy)->dead();
					f = true;
					break;
				}
			}
			if (!f)
				b++;
		}
	}

	void GameOnline::check_bullets_hit_block() {
		for (auto b = bullets.begin(); b != bullets.end();) {
			int x = ((*b)->getGlobalBounds().left +
				((*b)->getSpeed() > 0 ? (*b)->getGlobalBounds().width : 0)
				- getMap().getPosition().x) / rh->tile_size;
			if (!is_empty_block(x, (*b)->getGlobalBounds().top / rh->tile_size))
			{
				rm->delGraphic(*b);
				b = bullets.erase(b);
			}
			else
				b++;
		}
	}

	void GameOnline::check_coin() {
		for (auto c = coins.begin(); c != coins.end();) {
			if (p.getDrawable().getGlobalBounds().intersects((*c)->getDrawable().getGlobalBounds())) {
				p.score += (*c)->getScore();
				rm->delGraphic(&(*c)->getDrawable());
				c = coins.erase(c);
				rh->pickup.play();
			}
			else
				c++;
		}
	}

	void GameOnline::check_portal() {
		if (portal && px == (int)(p.getDrawable().getGlobalBounds().left - map.getPosition().x) / rh->tile_size &&
			py == (int)(p.getDrawable().getGlobalBounds().top +
				p.getDrawable().getGlobalBounds().height -
				map.getPosition().y) / rh->tile_size) {
			p.score += (level + 1) * 400;
			rh->view->move((toxs[nowp] - px) * 48, 0);
			p.NEWspawn((toxs[nowp] - px) * 48, (toys[nowp] - py) * 48);
			nowp++;
			if (nowp < xs.size()) {
				px = xs[nowp];
				py = ys[nowp];
			}
			else {
				{
					MySocket* sock = Core::CORE->sock;
					std::lock_guard<std::mutex> lock(sock->sout_mutex);
					sock->sout << "end " << 
				}
			}
		}
	}

	void GameOnline::check_player_enemy() {
		for (auto emy : enemies) {
			if (!emy->killed && emy->getDrawable().getGlobalBounds().intersects(p.getDrawable().getGlobalBounds())) {
				p.dead();
				break;
			}
		}
	}

	void GameOnline::check_enemy() {
		for (auto emy = enemies.begin(); emy != enemies.end();) {
			if ((*emy)->isDead()) {
				rm->delGraphic(&(*emy)->getDrawable());
				emy = enemies.erase(emy);
			}
			else
				emy++;
		}
	}

	void GameOnline::resetLvl() {
		setLvl(level);
	}

	void GameOnline::nextLvl() {
		setLvl(level + 1);
	}

	void GameOnline::setLvl(int lvl) {
		level = lvl;
		if (level == rh->no_of_maps) {
			rh->cheer.play();
			Core::CORE->switchMode(rpf::Mode::GAME_OVER);
			return;
		}
		rm->clear();
		for (Bullet* bullet : bullets) {
			free(bullet);
		}
		bullets.clear();
		for (Coin* coin : coins) {
			free(coin);
		}
		coins.clear();
		for (Enemy* enemy : enemies) {
			free(enemy);
		}
		enemies.clear();
		portal = nullptr;
		this->init_render();
		this->init_level();
		this->init_bg();
		init = true;
	}
}