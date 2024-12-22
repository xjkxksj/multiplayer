#pragma once

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <cmath>
#include "Game.h"

class Renderer {
private:
    Game& game;
public:
    Renderer(Game& game);

    void RenderMainScene();
    void RenderMiniMap();
    void RenderScene();
};