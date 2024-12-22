#pragma once

class Platform {
public:
    virtual void Setup(int width, int height) = 0;
    virtual void Cleanup() = 0;
    virtual void SwapBuffers() = 0;
    virtual void EventHandler() = 0;
};
