#include "SocketManager.hpp"
#include "Coin.hpp"
#include "MySocket.cpp"
//#include "GameOnline.hpp"

namespace rpf {

	SocketManager::SocketManager() : sock(new MySocket), in(sock->sin), out(sock->sout) {
	}

	void SocketManager::updatePlayer() {
		if (++ti == 2)
		{
			ti = 0;
			std::lock_guard<std::mutex> lock(game->obj_mutex);
			if (my_data != game->player_data) {
				my_data = game->player_data;
				std::lock_guard<std::mutex> lock_out(sock->sout_mutex);
				out << "player " << myid << " " << my_data.x << " " << my_data.y << " " << my_data.x_speed << " " << my_data.y_speed << std::endl;
			}
			if (game->shoot) {
				game->shoot = 0;
				out << "shoot " << myid << std::endl;
			}
			if (game->dead) {
				game->dead = 0;
				out << "dead " << myid << std::endl;
			}
			for (Bullet* b : game->bullets_out) {
				out << "bullet " << myid << " " << b->getX() << " " << b->getY() << " " << b->getSpeed() << std::endl;
			}
			game->bullets_out.clear();
			if (time != 0 && myid != sock->server_fd) {
				out << "finish " << myid << " " << time << std::endl;
			}
		}
	}

	int SocketManager::host() {
		sock->host(55072);
		return myid = sock->server_fd;
	}

	bool SocketManager::connect(std::string ip) {
		return sock->connect(ip, 55072);
	}

	void SocketManager::firstConnect() {
		std::lock_guard<std::mutex> lock(sock->sout_mutex);
		out << "ask" << std::endl;
	}

	std::string SocketManager::getline() {
		std::string receivedData;
		{
			std::lock_guard<std::mutex> lock(sock->sin_mutex);
			receivedData = in.str();
			in.str("");
			in.clear();
		}
		if (receivedData.empty()) {
			return "";
		}
		std::cout << "[Received]: " << receivedData << std::endl;
		int pos = 0;
		while (receivedData[pos] != '\n')
			pos++;
		{
			std::lock_guard<std::mutex> lock(sock->sin_mutex);
			in << receivedData.substr(pos + 1);
		}
		return receivedData.substr(0, pos + 1);
	}

	void SocketManager::loopServer() {
		std::thread([this]() { loopServerAsync(); }).detach();
	}

	void SocketManager::loopServerAsync() {
		while (!Core::GAME) {
			std::string receivedData = getline();
			handleRoomMsgServer(receivedData);
			{
				std::lock_guard<std::mutex> lockClient(sock->clients_mutex);
				if (!sock->leaved.empty()) {
					std::lock_guard<std::mutex> lockOut(sock->sout_mutex);
					int sz = sock->leaved.size();
					out << "quit " << sz << " ";
					while (!sock->leaved.empty())
						out << sock->leaved.front() << " ", sock->leaved.pop();
					out << std::endl;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}

	void SocketManager::handleRoomMsgServer(std::string receivedData) {
		if (receivedData.empty())
			return;
		std::string msg;
		std::stringstream handler(receivedData);
		handler >> msg;
		if (msg == "ask") {
			int sz = 0;
			int who = 111;
			{
				std::lock_guard<std::mutex> lockClient(sock->clients_mutex);
				sz = sock->clients.size() + 1;
				who = sock->joined.front();
				sock->joined.pop();
			}
			out << "join 1 " << who << std::endl;
			out << "players ";
			out << sz << " " << sock->server_fd << " ";
			for (auto& [_, fd] : sock->clients)
				out << fd << " ";
			out << who << std::endl;
		}
		else if (msg == "leave") {
			int who;
			handler >> who;
			Core::CORE->leaved.push_back(who);
			sock->clients.erase(who);
			out << "quit 1 " << who << std::endl;
		}
	}

	void SocketManager::loopServerGaming() {
		game = Core::GAME;
		std::thread([this]() { loopServerGamingAsync(); }).detach();
	}

	void SocketManager::loopServerGamingAsync() {
		int myid = sock->server_fd;
		data my_data;
		GameOnline* game = Core::GAME;
		int ti = 0;
		while (game) {
			if (firstEnd != 0 && std::time(NULL) - firstEnd > 10) {
				break;
			}
			updatePlayer();
			std::string receivedData = getline();
			handleGamingMsgServer(receivedData);
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		std::sort(ends.begin(), ends.end());
		int nth = 1;
		std::cout << "Game Over! below is the ranking." << std::endl;
		for (auto& [_time, id] : ends) {
			std::cout << "Top" << nth++ << ": " << _time << "s by player" << id << std::endl;
		}
	}

	void SocketManager::handleGamingMsgServer(std::string receivedData) {
		if (receivedData.empty())
			return;
		std::stringstream handler(receivedData);
		std::string msg;
		handler >> msg;
		if (msg == "player") {
			int id;
			handler >> id;
			float x, y, xspeed, yspeed;
			handler >> x >> y >> xspeed >> yspeed;
			if (id != myid)
			{
				std::lock_guard<std::mutex> lock(game->obj_mutex);
				game->poses[id] = { x,y,xspeed,yspeed };
			}
			std::lock_guard<std::mutex> lock_out(sock->sout_mutex);
			out << receivedData;
			out.flush();
		}
		else if (msg == "shoot") {
			int id;
			handler >> id;
			if (id != myid)
			{
				std::lock_guard<std::mutex> lock(game->obj_mutex);
				game->shoots[id] = id;
			}
			std::lock_guard<std::mutex> lock_out(sock->sout_mutex);
			out << receivedData;
			out.flush();
		}
		else if (msg == "dead") {
			int id;
			handler >> id;
			if (id != myid)
			{
				std::lock_guard<std::mutex> lock(game->obj_mutex);
				game->deads[id] = id;
			}
			std::lock_guard<std::mutex> lock_out(sock->sout_mutex);
			out << receivedData;
			out.flush();
		}
		else if (msg == "bullet") {
			int id;
			int x, y, speed;
			handler >> id >> x >> y >> speed;
			if (id != myid) {
				std::lock_guard<std::mutex> lock(game->obj_mutex);
				game->bullets_in.push_back(new Bullet(x, y, speed, game->rh));
			}
			std::lock_guard<std::mutex> lock_out(sock->sout_mutex);
			out << receivedData;
			out.flush();
		}
		else if (msg == "finish") {
			int id;
			float _time;
			handler >> id >> time;
			ends.push_back({ id, _time });
			if (firstEnd == 0)
				firstEnd = std::time(NULL);
		}
	}

	void SocketManager::loopClient(RoomMenu* menu) {
		std::thread([this, menu]() { loopClientAsync(menu); }).detach();
	}

	void SocketManager::loopClientAsync(RoomMenu* menu) {
		std::stringstream& sin = in;
		std::string msg;
		bool first = false;
		bool start = false;
		std::vector<int> ids;
		while (menu) {
			std::string receivedData = getline();
			if (receivedData.empty()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				continue;
			}
			std::stringstream in(receivedData);
			in >> msg;
			if (msg == "players") {
				if (first) {
					int N;
					in >> N;
					int tmp;
					for (int i = 0; i < N; i++)
						in >> tmp;
					in >> tmp;
					continue;//ignore
				}
				menu->leaved.push_back(0);
				first = true;
				int N;
				in >> N;
				int tmp;
				for (int i = 0; i < N; i++)
					in >> tmp, menu->joined.push_back(tmp);
				in >> tmp;
				menu->selfId = myid = tmp;
			}
			else if (msg == "join") {
				if (!first) {
					int N;
					in >> N;
					int tmp;
					for (int i = 0; i < N; i++)
						in >> tmp;
					continue;//ignore
				}
				int N;
				in >> N;
				int tmp;
				for (int i = 0; i < N; i++)
					in >> tmp, menu->joined.push_back(tmp);
			}
			else if (msg == "quit") {
				int N;
				in >> N;
				int tmp;
				for (int i = 0; i < N; i++)
					in >> tmp, menu->leaved.push_back(tmp);
			}
			else if (msg == "start") {
				int N;
				in >> N;
				for (int i = 0; i < N; i++) {
					int id;
					in >> id;
					ids.push_back(id);
				}
				Core::CORE->switchModeAsync(Mode::MULTI_GAME);
				start = true;
				break;
			}
			msg = "";
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		if (start && myid != -1) {
			game = Core::GAME;
			for (int& id : ids) {
				if (id == myid)
					continue;
				Core::GAME->poses[id] = { -1,-1 };
			}
			handleMsgClientGaming(myid);
		}
	}

	void SocketManager::handleMsgClientGaming(int myid) {
		std::stringstream& sin = in;
		std::string msg;
		bool first = false;
		bool start = false;
		data my_data;
		GameOnline* game = Core::GAME;
		game->resetLvlAsync();
		int ti = 0;
		while (game) {
			updatePlayer();
			std::string receivedData = getline();
			if (receivedData.empty()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				continue;
			}
			std::stringstream in(receivedData);
			in >> msg;
			if (msg == "player") {
				int id;
				in >> id;
				float x, y, xspeed, yspeed;
				in >> x >> y >> xspeed >> yspeed;
				if (id != myid)
				{
					std::lock_guard<std::mutex> lock(game->obj_mutex);
					game->poses[id] = { x,y,xspeed,yspeed };
				}
			}
			else if (msg == "shoot") {
				int id;
				in >> id;
				if (id != myid)
				{
					std::lock_guard<std::mutex> lock(game->obj_mutex);
					game->shoots[id] = id;
				}
			}
			else if (msg == "dead") {
				int id;
				in >> id;
				if (id != myid)
				{
					std::lock_guard<std::mutex> lock(game->obj_mutex);
					game->deads[id] = id;
				}
			}
			else if (msg == "bullet") {
				int id;
				int x, y, speed;
				in >> id >> x >> y >> speed;
				if (id != myid) {
					std::lock_guard<std::mutex> lock(game->obj_mutex);
					game->bullets_in.push_back(new Bullet(x, y, speed, game->rh));
				}
				std::lock_guard<std::mutex> lock_out(sock->sout_mutex);
			}
			msg = "";
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}

	void SocketManager::quitRoom() {
		std::lock_guard<std::mutex> lock(sock->sout_mutex);
		out << "leave " << myid << std::endl;
	}

	void SocketManager::finish(float time) {
		this->time = time;
		if (myid == sock->server_fd) {//I am server
			ends.push_back({ time, myid });
			if (firstEnd == 0)
				firstEnd = std::time(NULL);
		}
	}

}