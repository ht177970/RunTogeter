#pragma once
#include <sstream>
#include "Menu.hpp"
#include "GameOnline.hpp"
//#include "MySocket.cpp"

struct MySocket;

namespace rpf {
	class SocketManager {
	public:
		SocketManager();
		int host();
		bool connect(std::string ip);
		void loopServer();
		void loopClient(RoomMenu* menu);
		void finish(float time);
		void firstConnect();
		void loopServerGaming();
		void quitRoom();
		MySocket* sock;
	private:
		std::vector<std::pair<float, int>> ends;
		GameOnline* game;
		std::stringstream& in;
		std::stringstream& out;
		data my_data;
		float time = 0;
		int ti = 0;
		int myid = -1;
		int firstEnd = 0;
		std::string getline();
		void updatePlayer();
		void loopServerAsync();
		void loopServerGamingAsync();
		void loopClientAsync(RoomMenu* menu);
		void handleRoomMsgServer(std::string msg);
		void handleGamingMsgServer(std::string msg);
		void handleMsgClientGaming(int myid);
	};
}