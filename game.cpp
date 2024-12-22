#include "game.h"

Game::Game()
    : squareX(120.0f), squareY(120.0f), squareSpeed(5.0f), squareSize(50.0f),
      windowWidth(800), windowHeight(600), targetX(-1.0f), targetY(-1.0f),
      mapWidth(2000.0f), mapHeight(2000.0f), cameraX(0.0f), cameraY(0.0f),
      cameraSpeed(10.0f), mouseX(0), mouseY(0), transparency(1.0f),
      keyUp(false), keyDown(false), keyLeft(false), keyRight(false) {}

void Game::ResizeScene(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Game::HandleMovement() {
    if (keyUp && squareY > 0) squareY -= squareSpeed;
    if (keyDown && squareY + squareSize < mapHeight) squareY += squareSpeed;
    if (keyLeft && squareX > 0) squareX -= squareSpeed;
    if (keyRight && squareX + squareSize < mapWidth) squareX += squareSpeed;
}

void Game::MoveSquareToTarget() {
    if (targetX < 0 || targetY < 0) return;
    float deltaX = targetX - squareX;
    float deltaY = targetY - squareY;
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
    if (distance < squareSpeed) {
        squareX = targetX;
        squareY = targetY;
        targetX = -1;
        targetY = -1;
        return;
    }
    float dirX = deltaX / distance;
    float dirY = deltaY / distance;
    squareX += dirX * squareSpeed;
    squareY += dirY * squareSpeed;
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
