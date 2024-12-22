// #include <windows.h>
// #include <GL/gl.h>
// #include <GL/glu.h>
// #include <iostream>
// #include "platform.h"
// #include "scene_rendering.h"
// #include "game.h"

// class PlatformWindows : public Platform {
// private:
//     HDC hdc;  // Context dla okna
//     HGLRC hglrc;  // OpenGL rendering context
//     HWND hwnd;  // Handle okna
//     HINSTANCE hInstance;  // Handle aplikacji
//     Renderer renderer;
//     Game game;
// public:
//     PlatformWindows(Game game) : game(game) {}
//     LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) 
//     {
//             switch (msg) {
//             case WM_MOUSEMOVE: {
//                 game.mouseX = LOWORD(lParam);
//                 game.mouseY = HIWORD(lParam);
//                 return 0;
//             }
//             case WM_SIZE: {
//                 int windowWidth = LOWORD(lParam);
//                 int windowHeight = HIWORD(lParam);
//                 std::cout << "Width: " << windowWidth << ", Height: " << windowHeight << std::endl;
//                 game.ResizeScene(windowWidth, windowHeight);
//                 return 0;
//                 }
//             case WM_KEYDOWN:  // Klawisz wciśnięty
//                 switch (wParam) {
//                     case VK_UP:
//                         game.keyUp = true;
//                         break;
//                     case VK_DOWN:
//                         game.keyDown = true;
//                         break;
//                     case VK_LEFT:
//                         game.keyLeft = true;
//                         break;
//                     case VK_RIGHT:
//                         game.keyRight = true;
//                         break;
//                     case VK_CONTROL:
//                         game.transparency = 0.5f;
//                         break;
//                 }
//                 return 0;

//             case WM_KEYUP:  // Klawisz zwolniony
//                 switch (wParam) {
//                     case VK_UP:
//                         game.keyUp = false;
//                         break;
//                     case VK_DOWN:
//                         game.keyDown = false;
//                         break;
//                     case VK_LEFT:
//                         game.keyLeft = false;
//                         break;
//                     case VK_RIGHT:
//                         game.keyRight = false;
//                         break;
//                     case VK_CONTROL:
//                         game.transparency = 1.0f;
//                         break;
//                 }
//                 return 0;

//             case WM_LBUTTONDOWN: {
//                 int mouseX = LOWORD(lParam);
//                 int mouseY = HIWORD(lParam);
//                 game.targetX = static_cast<float>(mouseX + game.cameraX);
//                 game.targetY = static_cast<float>(mouseY + game.cameraY);
//                 return 0;
//                 }
//             case WM_DESTROY:
//                 PostQuitMessage(0);
//                 return 0;
//             default:
//                 return DefWindowProc(hwnd, msg, wParam, lParam);
//         }
//     }
//     void Setup(int width, int height) override 
//     {
//         WNDCLASS wc = {};
//         wc.lpfnWndProc = (WNDPROC)&WndProc;
//         wc.hInstance = hInstance;
//         wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
//         wc.lpszClassName = "oglversion";
//         wc.style = CS_OWNDC;
//         if (!RegisterClass(&wc)) {
//             std::cerr << "Failed to register window class" << std::endl;
//             return;
//         }

//         hwnd = CreateWindowEx(0, wc.lpszClassName, "OpenGL Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hInstance, 0);
//         if (!hwnd) {
//             std::cerr << "Failed to create window" << std::endl;
//             return;
//         }

//         ShowWindow(hwnd, SW_SHOW);

//         hdc = GetDC(hwnd);
//         PIXELFORMATDESCRIPTOR pfd = {0};
//         pfd.nSize = sizeof(pfd);
//         pfd.nVersion = 1;
//         pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
//         pfd.iPixelType = PFD_TYPE_RGBA;
//         pfd.cColorBits = 32;
//         pfd.cDepthBits = 24;
//         pfd.iLayerType = PFD_MAIN_PLANE;

//         int pf = ChoosePixelFormat(hdc, &pfd);
//         if (!pf) {
//             std::cerr << "Failed to choose pixel format" << std::endl;
//             return;
//         }

//         if (!SetPixelFormat(hdc, pf, &pfd)) {
//             std::cerr << "Failed to set pixel format" << std::endl;
//             return;
//         }

//         hglrc = wglCreateContext(hdc);
//         if (!hglrc) {
//             std::cerr << "Failed to create OpenGL rendering context" << std::endl;
//             return;
//         }

//         if (!wglMakeCurrent(hdc, hglrc)) {
//             std::cerr << "Failed to make OpenGL rendering context current" << std::endl;
//             return;
//         }

//         glEnable(GL_BLEND);
//         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//         glEnable(GL_DEPTH_TEST);
//         glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//         glMatrixMode(GL_PROJECTION);
//         glLoadIdentity();
//         gluPerspective(45.0, 800.0 / 600.0, 1.0, 100.0);
//         glMatrixMode(GL_MODELVIEW);
//         glLoadIdentity();

//         std::cout << "OpenGL setup completed" << std::endl;
//     }

//     void Cleanup() override 
//     {
//         wglMakeCurrent(hdc, 0);
//         wglDeleteContext(hglrc);
//         ReleaseDC(hwnd, hdc);
//         DestroyWindow(hwnd);
//     }

//     void SwapBuffers() override 
//     {
//         ::SwapBuffers(hdc);
//     }

//     void EventHandler() 
//     {
//         MSG msg;
//         if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
//             TranslateMessage(&msg);
//             DispatchMessage(&msg);
//         }
//     }

//     void Render() 
//     {
//         renderer.RenderScene();
//         SwapBuffers();
//     }
// };

#pragma once

#include <windows.h>
#include "game.h"
#include "renderer.h"
#include "platform.h"

class PlatformWindows : public Platform {
public:
    PlatformWindows(Game& game);
    void Setup(int width, int height) override;
    void EventHandler() override;
    void SwapBuffers() override;
    void Cleanup() override;
    void Render();

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    Game& game;
    HINSTANCE hInstance;
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
    Renderer renderer;
};

