// #include "platform_linux.h"

// Display* display;  // Połączenie z serwerem X
// Window root;  // Korzeniowy window w X
// Window window;  // Okno aplikacji
// GLXContext glContext;  // GLX rendering context
// XVisualInfo* visualInfo;  // Wybór wizualizacji
// Colormap colormap;  // Kolorowa mapa

// void SetupPlatform(int width, int height) {
//     // Połączenie z serwerem X
//     display = XOpenDisplay(NULL);
//     if (!display) {
//         std::cerr << "Cannot open X display!" << std::endl;
//         exit(1);
//     }

//     root = DefaultRootWindow(display);

//     // Ustalenie właściwego wizualu
//     GLint attribs[] = {GLX_RGBA, GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE, 24, None};
//     visualInfo = glXChooseVisual(display, 0, attribs);
//     if (!visualInfo) {
//         std::cerr << "No appropriate visual found!" << std::endl;
//         exit(1);
//     }

//     colormap = XCreateColormap(display, root, visualInfo->visual, AllocNone);

//     XSetWindowAttributes winAttribs;
//     winAttribs.colormap = colormap;
//     winAttribs.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

//     window = XCreateWindow(display, root, 0, 0, width, height, 0, 
//                            visualInfo->depth, InputOutput, visualInfo->visual, 
//                            CWColormap | CWEventMask, &winAttribs);

//     XMapWindow(display, window);
//     XStoreName(display, window, "OpenGL Window");

//     // Tworzenie kontekstu GLX
//     glContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
//     glXMakeCurrent(display, window, glContext);

//     // Ustawienia OpenGL
//     glEnable(GL_DEPTH_TEST);
//     glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
// }

// void CleanupPlatform() {
//     glXMakeCurrent(display, None, NULL);
//     glXDestroyContext(display, glContext);
//     XDestroyWindow(display, window);
//     XCloseDisplay(display);
// }

// void SwapBuffersPlatform() {
//     glXSwapBuffers(display, window);
// }

// bool HandleEvents() {
//     XEvent event;
//     while (XPending(display)) {
//         XNextEvent(display, &event);
//         switch (event.type) {
//             case KeyPress:
//                 return false;  // Zakończenie programu po naciśnięciu klawisza
//             case ClientMessage:
//                 if ((Atom)event.xclient.data.l[0] == XInternAtom(display, "WM_DELETE_WINDOW", False)) {
//                     return false;  // Zakończenie programu po zamknięciu okna
//                 }
//                 break;
//         }
//     }
//     return true;
// }

#include "platform_linux.h"
#include <iostream>

PlatformLinux::PlatformLinux(Game& game) : game(game), display(nullptr), window(0), glContext(0), visualInfo(nullptr), colormap(0), renderer(game) {}

void PlatformLinux::Setup(int width, int height) {
    display = XOpenDisplay(NULL);
    if (!display) {
        std::cerr << "Cannot open X display!" << std::endl;
        exit(1);
    }

    root = DefaultRootWindow(display);

    GLint attribs[] = {GLX_RGBA, GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE, 24, None};
    visualInfo = glXChooseVisual(display, 0, attribs);
    if (!visualInfo) {
        std::cerr << "No appropriate visual found!" << std::endl;
        exit(1);
    }

    colormap = XCreateColormap(display, root, visualInfo->visual, AllocNone);

    XSetWindowAttributes winAttribs;
    winAttribs.colormap = colormap;
    winAttribs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

    window = XCreateWindow(display, root, 0, 0, width, height, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWColormap | CWEventMask, &winAttribs);

    XMapWindow(display, window);
    XStoreName(display, window, "OpenGL Window");

    glContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
    glXMakeCurrent(display, window, glContext);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 1.0, 100.0);
}

void PlatformLinux::EventHandler() {
    while (XPending(display)) {
        XEvent event;
        XNextEvent(display, &event);

        switch (event.type) {
            case KeyPress: {
                KeySym key = XLookupKeysym(&event.xkey, 0);
                switch (key) {
                    case XK_w: game.keyUp = true; break;
                    case XK_s: game.keyDown = true; break;
                    case XK_a: game.keyLeft = true; break;
                    case XK_d: game.keyRight = true; break;
                    case XK_Control_L: game.transparency = 0.5f; break;
                }
                break;
            }
            case KeyRelease: {
                KeySym key = XLookupKeysym(&event.xkey, 0);
                switch (key) {
                    case XK_w: game.keyUp = false; break;
                    case XK_s: game.keyDown = false; break;
                    case XK_a: game.keyLeft = false; break;
                    case XK_d: game.keyRight = false; break;
                    case XK_Control_L: game.transparency = 1.0f; break;
                }
                break;
            }
            case MotionNotify:
                game.mouseX = event.xmotion.x;
                game.mouseY = event.xmotion.y;
                break;
            case ButtonPress:
                game.targetX = static_cast<float>(event.xbutton.x + game.cameraX);
                game.targetY = static_cast<float>(event.xbutton.y + game.cameraY);
                break;
            case ClientMessage:
                if ((Atom)event.xclient.data.l[0] == XInternAtom(display, "WM_DELETE_WINDOW", False)) {
                    exit(0);
                }
                break;
        }
    }
}

void PlatformLinux::SwapBuffers() {
    glXSwapBuffers(display, window);
}

void PlatformLinux::Cleanup() {
    glXMakeCurrent(display, None, NULL);
    glXDestroyContext(display, glContext);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void PlatformLinux::Render() {
    renderer.RenderScene();
    SwapBuffers();
}
