#pragma once

#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <mutex>
#include <unordered_map>

class Player {
public:
    int id;
    float x, y;
};

class Game {
public:
    Game();
    void ResizeScene(int width, int height);
    void HandleMovement(int playerId);
    void MoveSquareToTarget(int playerId);
    void HandleCameraMovement();
    int AddPlayer(int playerId);

    // Zmienne stanu gry
    std::vector<Player> players;
    std::mutex playersMutex; // Mutex do synchronizacji dostępu do listy graczy
    std::unordered_map<int, Player> positionBuffer; // Buforowanie pozycji graczy
    std::mutex positionBufferMutex; // Mutex do synchronizacji dostępu do bufora pozycji
    float squareSpeed;
    float squareSize;
    int windowWidth;
    int windowHeight;
    float targetX;
    float targetY;
    float mapWidth;
    float mapHeight;
    float cameraX;
    float cameraY;
    float cameraSpeed;
    int mouseX, mouseY;
    float transparency;
    bool keyUp, keyDown, keyLeft, keyRight;
};