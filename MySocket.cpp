#include <sstream>
#include <thread>
#include <unordered_map>
#include <mutex>
#include <string>
#include <iostream>
#include <unistd.h>
#include "unp.h"

struct MySocket {
    std::stringstream sin, sout;
    int sockfd;
    std::unordered_map<int, std::string> clients; // Store client socket file descriptors and their ip address
    std::mutex mtx; // Mutex to synchronize access to shared resources

    MySocket() : sockfd(-1) {}

    void host(int port) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("Socket creation failed");
            return;
        }

        struct sockaddr_in servaddr {};
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(port);

        if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
            perror("Bind failed");
            close(sockfd);
            return;
        }

        if (listen(sockfd, 10) < 0) {
            perror("Listen failed");
            close(sockfd);
            return;
        }

        std::cout << "Server listening on port " << port << std::endl;

        std::thread([this]() { acceptClients(); }).detach();
        std::thread([this]() { handleOutgoing(); }).detach();
    }

    bool connect(const std::string& ip, int port) {//modified from void to boolean for returning connect state
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("Socket creation failed");
            return 0;
        }

        struct sockaddr_in servaddr {};
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);
        if (inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr) <= 0) {
            perror("Invalid address");
            close(sockfd);
            return 0;
        }

        if (::connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
            perror("Connection failed");
            close(sockfd);
            return 0;
        }

        std::cout << "Connected to server at " << ip << ":" << port << std::endl;

        std::thread([this]() { handleIncoming(); }).detach();
        std::thread([this]() { handleOutgoing(); }).detach();
        return 1;
    }

private:
    void acceptClients() {
        while (true) {
            struct sockaddr_in cliaddr {};
            socklen_t clilen = sizeof(cliaddr);
            int connfd = accept(sockfd, (SA*)&cliaddr, &clilen);
            if (connfd < 0) {
                perror("Accept failed");
                continue;
            }

            char clientIp[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &cliaddr.sin_addr, clientIp, INET_ADDRSTRLEN);
            std::string clientAddress = std::string(clientIp) + ":" + std::to_string(ntohs(cliaddr.sin_port));

            std::cout << "New client connected: " << clientAddress << std::endl;

            {
                std::lock_guard<std::mutex> lock(mtx);
                clients[connfd] = clientAddress;
            }

            std::thread([this, connfd]() { handleClient(connfd); }).detach();
        }
    }

    void handleClient(int connfd) {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        while (true) {
            int n = read(connfd, buffer, sizeof(buffer));
            if (n <= 0) {
                close(connfd);
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    clients.erase(connfd);
                }
                std::cout << "Client disconnected" << std::endl;
                break;
            }
            buffer[n] = '\0';
            {
                std::lock_guard<std::mutex> lock(mtx);
                sin << buffer;
                for (const auto& client : clients) {
                    if (client.first != connfd) {
                        write(client.first, buffer, n);
                    }
                }
            }
        }
    }

    void handleIncoming() {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        while (true) {
            int n = read(sockfd, buffer, sizeof(buffer));
            if (n <= 0) {
                std::cout << "Disconnected from server" << std::endl;
                close(sockfd);
                break;
            }

            buffer[n] = '\0';
            sin << buffer;
        }
    }

    void handleOutgoing() {
        std::string line;
        while (true) {
            if (std::getline(sout, line)) {
                line += '\n';

                {
                    std::lock_guard<std::mutex> lock(mtx);
                    if (sockfd != -1) {
                        write(sockfd, line.c_str(), line.size());
                    }

                    for (const auto& client : clients) {
                        write(client.first, line.c_str(), line.size());
                    }
                }

                sin << line;
            }
        }
    }
};
