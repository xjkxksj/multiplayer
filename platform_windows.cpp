// #include <windows.h>
// #include <GL/gl.h>
// #include <GL/glu.h>
// #include <iostream>

// HDC hdc;  // Context dla okna
// HGLRC hglrc;  // OpenGL rendering context
// HWND hwnd;  // Handle okna
// HINSTANCE hInstance;  // Handle aplikacji

// LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
//     switch (msg) {
//         case WM_DESTROY:
//             PostQuitMessage(0);
//             break;
//         default:
//             return DefWindowProc(hwnd, msg, wParam, lParam);
//     }
//     return 0;
// }

// void SetupPlatform(int width, int height) {
//     WNDCLASS wc = {0};
//     wc.lpfnWndProc = WndProc;
//     wc.hInstance = GetModuleHandle(NULL);
//     wc.lpszClassName = "OpenGL_Window_Class";
//     RegisterClass(&wc);

//     hwnd = CreateWindowEx(0, "OpenGL_Window_Class", "OpenGL Window", WS_OVERLAPPEDWINDOW,
//                           CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, wc.hInstance, NULL);

//     hdc = GetDC(hwnd);

//     PIXELFORMATDESCRIPTOR pfd = {0};
//     pfd.nSize = sizeof(pfd);
//     pfd.nVersion = 1;
//     pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
//     pfd.iPixelType = PFD_TYPE_RGBA;
//     pfd.cColorBits = 32;
//     pfd.cRedBits = 8;
//     pfd.cGreenBits = 8;
//     pfd.cBlueBits = 8;
//     pfd.cAlphaBits = 8;

//     int pixelFormat = ChoosePixelFormat(hdc, &pfd);
//     SetPixelFormat(hdc, pixelFormat, &pfd);

//     hglrc = wglCreateContext(hdc);
//     wglMakeCurrent(hdc, hglrc);

//     glEnable(GL_DEPTH_TEST);
//     glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//     ShowWindow(hwnd, SW_SHOW);
//     UpdateWindow(hwnd);
// }

// void CleanupPlatform() {
//     wglMakeCurrent(hdc, NULL);
//     wglDeleteContext(hglrc);
//     ReleaseDC(hwnd, hdc);
//     DestroyWindow(hwnd);
// }

// void SwapBuffersPlatform() {
//     SwapBuffers(hdc);
// }

// bool HandleEvents() {
//     MSG msg;
//     if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
//         TranslateMessage(&msg);
//         DispatchMessage(&msg);
//         if (msg.message == WM_QUIT) return false;
//     }
//     return true;
// }

#include "platform_windows.h"
#include <iostream>

PlatformWindows::PlatformWindows(Game& game) : game(game), hInstance(GetModuleHandle(NULL)), hwnd(NULL), hdc(NULL), hglrc(NULL), renderer(game) {}

void PlatformWindows::Setup(int width, int height) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = PlatformWindows::WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName = "oglversion";
    wc.style = CS_OWNDC;
    if (!RegisterClass(&wc)) {
        std::cerr << "Failed to register window class" << std::endl;
        return;
    }

    hwnd = CreateWindowEx(0, wc.lpszClassName, "OpenGL Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hInstance, this);
    if (!hwnd) {
        std::cerr << "Failed to create window" << std::endl;
        return;
    }

    ShowWindow(hwnd, SW_SHOW);

    hdc = GetDC(hwnd);
    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pf = ChoosePixelFormat(hdc, &pfd);
    if (!SetPixelFormat(hdc, pf, &pfd)) {
        std::cerr << "Failed to set pixel format" << std::endl;
        return;
    }

    hglrc = wglCreateContext(hdc);
    if (!hglrc) {
        std::cerr << "Failed to create OpenGL context" << std::endl;
        return;
    }

    if (!wglMakeCurrent(hdc, hglrc)) {
        std::cerr << "Failed to activate OpenGL context" << std::endl;
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 1.0, 100.0);
}

void PlatformWindows::EventHandler() {
    MSG msg = {};
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void PlatformWindows::SwapBuffers() {
    ::SwapBuffers(hdc);
}

void PlatformWindows::Cleanup() {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hglrc);
    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);
}

LRESULT CALLBACK PlatformWindows::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    PlatformWindows* platform;
    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        platform = (PlatformWindows*)cs->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)platform);
    } else {
        platform = (PlatformWindows*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (platform) {
        return platform->HandleMessage(hwnd, uMsg, wParam, lParam);
    } else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

LRESULT PlatformWindows::HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_SIZE:
            game.windowWidth = LOWORD(lParam);
            game.windowHeight = HIWORD(lParam);
            game.ResizeScene(game.windowWidth, game.windowHeight);
            return 0;
        case WM_KEYDOWN:
            switch (wParam) {
                case VK_UP: game.keyUp = true; break;
                case VK_DOWN: game.keyDown = true; break;
                case VK_LEFT: game.keyLeft = true; break;
                case VK_RIGHT: game.keyRight = true; break;
                case VK_CONTROL: game.transparency = 0.5f; break;
            }
            return 0;
        case WM_KEYUP:
            switch (wParam) {
                case VK_UP: game.keyUp = false; break;
                case VK_DOWN: game.keyDown = false; break;
                case VK_LEFT: game.keyLeft = false; break;
                case VK_RIGHT: game.keyRight = false; break;
                case VK_CONTROL: game.transparency = 1.0f; break;
            }
            return 0;
        case WM_MOUSEMOVE:
            game.mouseX = LOWORD(lParam);
            game.mouseY = HIWORD(lParam);
            return 0;
        case WM_LBUTTONDOWN:
            game.targetX = static_cast<float>(LOWORD(lParam) + game.cameraX);
            game.targetY = static_cast<float>(HIWORD(lParam) + game.cameraY);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void PlatformWindows::Render() 
{
    renderer.RenderScene();
    SwapBuffers();
}