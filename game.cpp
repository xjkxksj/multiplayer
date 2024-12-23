#include "game.h"
#include <iostream>
#include <algorithm>

Game::Game()
    : squareSpeed(5.0f), squareSize(50.0f), windowWidth(800), windowHeight(600),
      targetX(-1.0f), targetY(-1.0f), mapWidth(2000.0f), mapHeight(2000.0f),
      cameraX(0.0f), cameraY(0.0f), cameraSpeed(10.0f), mouseX(0), mouseY(0),
      transparency(1.0f), keyUp(false), keyDown(false), keyLeft(false), keyRight(false) {}

void Game::ResizeScene(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int Game::AddPlayer(int playerId) {
    Player newPlayer;
    newPlayer.id = playerId; // Użyj ID otrzymanego od serwera
    newPlayer.x = 120.0f; // Początkowa pozycja X
    newPlayer.y = 120.0f; // Początkowa pozycja Y
    std::lock_guard<std::mutex> lock(playersMutex);
    players.push_back(newPlayer);
    std::cout << "Player added with ID: " << newPlayer.id << std::endl;
    return newPlayer.id;
}

void Game::HandleMovement(int playerId) {
    std::lock_guard<std::mutex> lock(playersMutex);
    auto it = std::find_if(players.begin(), players.end(), [playerId](const Player& player) {
        return player.id == playerId;
    });

    if (it == players.end()) {
        std::cerr << "Player not found" << std::endl;
        return;
    }

    Player& player = *it;
    if (keyUp && player.y > 0) player.y -= squareSpeed;
    if (keyDown && player.y + squareSize < mapHeight) player.y += squareSpeed;
    if (keyLeft && player.x > 0) player.x -= squareSpeed;
    if (keyRight && player.x + squareSize < mapWidth) player.x += squareSpeed;
}

void Game::MoveSquareToTarget(int playerId) {
    std::lock_guard<std::mutex> lock(playersMutex);
    auto it = std::find_if(players.begin(), players.end(), [playerId](const Player& player) {
        return player.id == playerId;
    });

    if (it == players.end()) {
        std::cerr << "Player not found" << std::endl;
        return;
    }

    Player& player = *it;
    if (targetX < 0 || targetY < 0) return;
    float deltaX = targetX - player.x;
    float deltaY = targetY - player.y;
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
    if (distance < squareSpeed) {
        player.x = targetX;
        player.y = targetY;
        targetX = -1;
        targetY = -1;
        return;
    }
    float dirX = deltaX / distance;
    float dirY = deltaY / distance;
    player.x += dirX * squareSpeed;
    player.y += dirY * squareSpeed;
}

void Game::HandleCameraMovement() {
    int margin = 50;

    if (mouseX < margin && cameraX > 0) {
        cameraX -= cameraSpeed;
    }
    if (mouseX > windowWidth - margin && cameraX + windowWidth < mapWidth) {
        cameraX += cameraSpeed;
    }
    if (mouseY < margin && cameraY > 0) {
        cameraY -= cameraSpeed;
    }
    if (mouseY > windowHeight - margin && cameraY + windowHeight < mapHeight) {
        cameraY += cameraSpeed;
    }
}