#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <sstream>
#include "game.h"

#pragma comment(lib, "ws2_32.lib")

std::vector<Player> gameState;
std::vector<SOCKET> clientSockets;
std::mutex gameStateMutex;
std::mutex idMutex; // Mutex do synchronizacji dostępu do nextPlayerId
int nextPlayerId = 0; // Zmienna do przechowywania następnego dostępnego identyfikatora gracza

void NetworkHandler(SOCKET clientSocket) {
    int playerId;
    {
        std::lock_guard<std::mutex> lock(idMutex);
        playerId = nextPlayerId++; // Nadaj unikalne ID na podstawie licznika
        std::cout << "Assigned player ID: " << playerId << std::endl; // Logowanie przypisania ID
    }

    // Wyślij ID do klienta
    std::ostringstream oss;
    oss << "ID " << playerId << "\n";
    std::string idMessage = oss.str();
    send(clientSocket, idMessage.c_str(), idMessage.size(), 0);

    char buffer[256];
    while (true) {
        int recvSize = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (recvSize <= 0) {
            std::cerr << "Client closed the connection or error: " << WSAGetLastError() << std::endl;
            break;
        }
        buffer[recvSize] = '\0';
        std::istringstream iss(buffer);
        std::string command;
        iss >> command;

        if (command == "MOVE") {
            int id;
            float x, y;
            iss >> id >> x >> y;
            {
                std::lock_guard<std::mutex> lock(gameStateMutex);
                bool playerExists = false;
                for (auto& p : gameState) {
                    if (p.id == id) {
                        p.x = x;
                        p.y = y;
                        playerExists = true;
                        break;
                    }
                }
                if (!playerExists) {
                    Player newPlayer{id, x, y};
                    gameState.push_back(newPlayer);
                }
            }
        }

        // Wyślij zaktualizowany stan gry do wszystkich klientów
        {
            std::lock_guard<std::mutex> lock(gameStateMutex);
            std::ostringstream oss;
            for (const auto& player : gameState) {
                oss << "UPDATE " << player.id << " " << player.x << " " << player.y << "\n";
            }
            std::string updateMessage = oss.str();
            for (SOCKET socket : clientSockets) {
                send(socket, updateMessage.c_str(), updateMessage.size(), 0);
            }
        }
    }
    std::cerr << "Client disconnected: " << WSAGetLastError() << std::endl;
    closesocket(clientSocket);
}

int main() {
    WSADATA wsa;
    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    // Inicjalizacja Winsock
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Tworzenie gniazda serwera
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Could not create socket: " << WSAGetLastError() << std::endl;
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    listen(serverSocket, 1);

    std::cout << "Waiting for a connection..." << std::endl;

    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }
        std::cout << "Client connected!" << std::endl;

        // Dodaj nowy socket klienta do listy
        {
            std::lock_guard<std::mutex> lock(gameStateMutex);
            clientSockets.push_back(clientSocket);
        }

        // Uruchom wątek do obsługi nowego klienta
        std::thread(NetworkHandler, clientSocket).detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}