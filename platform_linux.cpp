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
