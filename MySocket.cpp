#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <sstream>
#include <thread>
#include <unordered_map>
#include <mutex>
#include <string>
#include <iostream>
#include <cstring>
#include <queue>

#include "Core.hpp"

//struct data {
//    float x = 0, y = 0;
//    float x_speed = 0, y_speed = 0;
//
//    bool operator!=(const data& ot) {
//        return x != ot.x || y != ot.y || x_speed != ot.x_speed || y_speed  != ot.y_speed;
//    }
//    //bool dead;
//};

class MySocket {
public:
    std::stringstream sin;   // 收到的資料
    std::stringstream sout;  // 要傳送的資料

    MySocket() : server_fd(-1) {}

    ~MySocket() {
        if (server_fd != -1) {
            close(server_fd);
        }
        for (auto& [_, fd] : clients) {
            close(fd);
        }
    }

    // Host Function: 開啟伺服器
    void host(int port) {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
            perror("Socket creation failed");
            return;
        }

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);
        // addr reuse
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("Bind failed");
            close(server_fd);
            return;
        }

        if (listen(server_fd, 5) < 0) {
            perror("Listen failed");
            close(server_fd);
            return;
        }

        std::cout << "Server listening on port " << port << std::endl;

        // Accept new clients
        std::thread(&MySocket::acceptClients, this).detach();

        // Broadcast loop
        std::thread(&MySocket::broadcastLoop, this).detach();
    }

    // Connect Function: 作為客戶端連線到伺服器
    bool connect(const std::string& ip, int port) {
        int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd < 0) {
            perror("Socket creation failed");
            return false;
        }

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);

        if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
            perror("Invalid address/Address not supported");
            close(sock_fd);
            return false;
        }

        if (::connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection failed");
            close(sock_fd);
            return false;
        }

        std::cout << "Connected to server " << ip << ":" << port << std::endl;

        // 接收資料的執行緒
        std::thread([this, sock_fd]() {
            char buffer[1024];
            while (true) {
                memset(buffer, 0, sizeof(buffer));
                int bytes_received = recv(sock_fd, buffer, sizeof(buffer), 0);
                if (bytes_received <= 0) {
                    std::cerr << "Server connection lost." << std::endl;
                    close(sock_fd);
                    break;
                }
                std::lock_guard<std::mutex> lock(sin_mutex);
                sin << buffer;
            }
            }).detach();

        // 傳送資料的執行緒
        std::thread([this, sock_fd]() {
            while (true) {
                std::string data;
                {
                    std::lock_guard<std::mutex> lock(sout_mutex);
                    data = sout.str().c_str();
                    sout.str("");
                    sout.clear();
                }
                if (!data.empty()) {
                    send(sock_fd, data.c_str(), data.size(), 0);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            }).detach();

        return true;
    }
    int server_fd;  // Server socket file descriptor
    std::unordered_map<int, int> clients;  // 客戶端的連線
    std::queue<int> joined, leaved;
    std::mutex clients_mutex;  // 保護 clients 資料結構
    std::mutex sin_mutex;      // 保護 sin
    std::mutex sout_mutex;     // 保護 sout

    // 接受客戶端連線
    void acceptClients() {
        while (true) {
            sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
            if (client_fd < 0) {
                perror("Accept failed");
                continue;
            }

            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients[client_fd] = client_fd;
                joined.push(client_fd);
                rpf::Core::CORE->joined.push_back(client_fd);
            }

            std::cout << "New client connected." << std::endl;

            // 處理客戶端資料
            std::thread(&MySocket::handleClient, this, client_fd).detach();
        }
    }

    // 處理客戶端資料
    void handleClient(int client_fd) {
        char buffer[1024];
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) {
                std::cerr << "Client disconnected." << std::endl;
                close(client_fd);
                {
                    std::lock_guard<std::mutex> lock(clients_mutex);
                    clients.erase(client_fd);
                    leaved.push(client_fd);
                    rpf::Core::CORE->leaved.push_back(client_fd);
                }
                break;
            }

            // 廣播收到的訊息
            {
                std::lock_guard<std::mutex> lock(sin_mutex);
                sin << buffer;
            }
        }
    }

    // 廣播伺服器收到的資料
    void broadcastLoop() {
        while (true) {
            std::string data;
            {
                std::lock_guard<std::mutex> lock(sout_mutex);
                data = sout.str().c_str();
                sout.str("");
                sout.clear();
            }

            if (!data.empty()) {
                std::lock_guard<std::mutex> lock(clients_mutex);
                for (const auto& [_, client_fd] : clients) {
                    send(client_fd, data.c_str(), data.size(), 0);
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
};
