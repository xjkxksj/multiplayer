#pragma once

#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

class Game {
public:
    Game();
    void ResizeScene(int width, int height);
    void HandleMovement();
    void MoveSquareToTarget();
    void HandleCameraMovement();

    // Zmienne stanu gry
    float squareX;
    float squareY;
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
