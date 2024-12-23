#include "renderer.h"

Renderer::Renderer(Game& game) : game(game) {}

void Renderer::RenderMainScene() {
    for (const auto& player : game.players) {
        float x1 = player.x;
        float y1 = player.y;
        float x2 = player.x + game.squareSize;
        float y2 = player.y + game.squareSize;

        glBegin(GL_QUADS);
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f); // Nieprzezroczysty czerwony kwadrat
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
        glEnd();
    }

    glDisable(GL_DEPTH_TEST);

    for (float y = 110.0f; y < game.mapHeight; y += 550.0f) {
        for (float x = 110.0f; x < game.mapWidth; x += 550.0f) {
            float greenCenterX = x + 27.5f; // 27.5 = 55 / 2
            float greenCenterY = y + 27.5f;

            float redCenterX = game.players[0].x + game.squareSize / 2.0f;
            float redCenterY = game.players[0].y + game.squareSize / 2.0f;

            float deltaX = greenCenterX - redCenterX;
            float deltaY = greenCenterY - redCenterY;
            float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

            float thresholdDistance = 200.0f; // Próg odległości
            float currentTransparency = (distance < thresholdDistance) ? 0.1f : 1.0f;

            glBegin(GL_QUADS);
            glColor4f(0.2f, 0.6f, 0.3f, currentTransparency);
            glVertex2f(x, y);
            glVertex2f(x + 55.0f, y);
            glVertex2f(x + 55.0f, y + 55.0f);
            glVertex2f(x, y + 55.0f);
            glEnd();
        }
    }
    glEnable(GL_DEPTH_TEST);
}

void Renderer::RenderMiniMap() {
    glViewport(game.windowWidth - 200, game.windowHeight - 200, 200, 200); // Prawy górny róg
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, game.mapWidth, game.mapHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (const auto& player : game.players) {
        float miniSquareX = player.x;
        float miniSquareY = player.y;
        float miniSquareSize = game.squareSize;

        glBegin(GL_QUADS);
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f); // Kwadrat gracza
        glVertex2f(miniSquareX, miniSquareY);
        glVertex2f(miniSquareX + miniSquareSize, miniSquareY);
        glVertex2f(miniSquareX + miniSquareSize, miniSquareY + miniSquareSize);
        glVertex2f(miniSquareX, miniSquareY + miniSquareSize);
        glEnd();
    }

    float cameraViewX = game.cameraX;
    float cameraViewY = game.cameraY;
    float cameraViewWidth = game.windowWidth;
    float cameraViewHeight = game.windowHeight;

    glBegin(GL_LINE_LOOP);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Prostokąt widoku kamery
    glVertex2f(cameraViewX, cameraViewY);
    glVertex2f(cameraViewX + cameraViewWidth, cameraViewY);
    glVertex2f(cameraViewX + cameraViewWidth, cameraViewY + cameraViewHeight);
    glVertex2f(cameraViewX, cameraViewY + cameraViewHeight);
    glEnd();

    glDisable(GL_DEPTH_TEST); // Wyłącz test głębokości
    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 1.0f, 0.01f);
    for(int i = 0; i < 360; i++){
        float x = game.players[0].x + 200 * cos(i);
        float y = game.players[0].y + 200 * sin(i);
        glVertex2f(x, y);
    }
    glEnd();

    glEnable(GL_DEPTH_TEST); // Włącz test głębokości

    glViewport(0, 0, game.windowWidth, game.windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, game.windowWidth, game.windowHeight, 0, -1, 1);
}

void Renderer::RenderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glTranslatef(-game.cameraX, -game.cameraY, 0.0f);
    RenderMainScene();
    glPopMatrix();
    RenderMiniMap();
}