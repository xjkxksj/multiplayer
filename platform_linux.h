#pragma once

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <iostream>
#include "platform.h"
#include "renderer.h"

class PlatformLinux : public Platform {
private:
    Display* display;
    Window root;
    Window window;
    GLXContext glContext;
    XVisualInfo* visualInfo;
    Colormap colormap;
    Renderer renderer;
    Game& game;

public:
    PlatformLinux(Game& game);
    void Setup(int width, int height) override;
    void Cleanup() override;
    void SwapBuffers() override;
    void EventHandler() override;
    void Render();
};
