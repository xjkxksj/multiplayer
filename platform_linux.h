#include <X11/Xlib.h>
// #include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <iostream>
#include "platform.h"
#include "renderer.h"

class PlatformLinux : public Platform {
private:
    Display* display;  // Połączenie z serwerem X
    Window root;  // Korzeniowy window w X
    Window window;  // Okno aplikacji
    GLXContext glContext;  // GLX rendering context
    XVisualInfo* visualInfo;  // Wybór wizualizacji
    Colormap colormap;  // Kolorowa mapa
    Renderer renderer;

public:
    void Setup(int width, int height) override 
    {
        // Połączenie z serwerem X
        display = XOpenDisplay(NULL);
        if (!display) {
            std::cerr << "Cannot open X display!" << std::endl;
            exit(1);
        }

        root = DefaultRootWindow(display);

        // Ustalenie właściwego wizualu
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

        window = XCreateWindow(display, root, 0, 0, width, height, 0, 
                               visualInfo->depth, InputOutput, visualInfo->visual, 
                               CWColormap | CWEventMask, &winAttribs);

        XMapWindow(display, window);
        XStoreName(display, window, "OpenGL Window");

        // Tworzenie kontekstu GLX
        glContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
        glXMakeCurrent(display, window, glContext);

        // Ustawienia OpenGL
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, 800.0 / 600.0, 1.0, 100.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void Cleanup() override 
    {
        glXMakeCurrent(display, None, NULL);
        glXDestroyContext(display, glContext);
        XDestroyWindow(display, window);
        XCloseDisplay(display);
    }

    void SwapBuffers() override 
    {
        glXSwapBuffers(display, window);
    }

    void EventHandler() 
    {
        while (XPending(display)) {
            XEvent event;
            XNextEvent(display, &event);

            switch (event.type) {
                case KeyPress: {
                    KeySym key = XLookupKeysym(&event.xkey, 0);
                    switch (key)
                    {
                    case XK_w:
                        keyUp = true;
                        break;
                    case XK_:
                        keyDown = true;
                        break;
                    case VK_LEFT:
                        keyLeft = true;
                        break;
                    case VK_RIGHT:
                        keyRight = true;
                        break;
                    case VK_CONTROL:
                        transparency = 0.5f;
                        break;
                    
                    default:
                        break;
                    }
                    if (key == XK_Escape) {
                        return;
                    }
                    break;
                }
                case KeyRelease: {
                    KeySym key = XLookupKeysym(&event.xkey, 0);
                    std::cout << "Key released: " << XKeysymToString(key) << std::endl;
                    break;
                }
                case ButtonPress: {
                    std::cout << "Mouse button pressed: " << event.xbutton.button
                              << " at (" << event.xbutton.x << ", " << event.xbutton.y << ")" << std::endl;
                    break;
                }
                case MotionNotify: {
                    std::cout << "Mouse moved to: (" << event.xmotion.x << ", " << event.xmotion.y << ")" << std::endl;
                    break;
                }
                case ClientMessage: {
                    // Obsługa zamknięcia okna (jeśli ustawione)
                    Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
                    if (static_cast<Atom>(event.xclient.data.l[0]) == wmDeleteMessage) {
                        return;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    void Render() 
    {
        renderer.RenderScene();
        SwapBuffers();
    }
};