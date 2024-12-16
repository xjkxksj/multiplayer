#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <cmath>

// Prototypy funkcji
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void SetupOpenGL(HWND hwnd);
void RenderScene(HDC hdc);
void RenderMainScene();
void RenderMiniMap();
void ResizeScene(int width, int height);
void HandleMovement();
void MoveSquareToTarget();
void HandleCameraMovement();


HGLRC hglrc; // Kontekst renderowania OpenGL
float squareX = 120.0f;   // Początkowa pozycja X kwadratu
float squareY = 120.0f;   // Początkowa pozycja Y kwadratu
float squareSpeed = 5.0f; // Prędkość poruszania kwadratu
float squareSize = 50.0f; // Rozmiar kwadratu
int windowWidth = 800;  // Domyślna szerokość
int windowHeight = 600; // Domyślna wysokość
float targetX = -1.0f; // Docelowe X (zainicjalizowane na wartość poza ekranem)
float targetY = -1.0f; // Docelowe Y
float mapWidth = 2000.0f;  // Szerokość planszy
float mapHeight = 2000.0f; // Wysokość planszy
float cameraX = 0.0f;      // Pozycja kamery X
float cameraY = 0.0f;      // Pozycja kamery Y
float cameraSpeed = 10.0f; // Prędkość przesuwania widoku
int mouseX = 0, mouseY = 0;
float transparency = 1.0f;

// Stany klawiszy
bool keyUp = false, keyDown = false, keyLeft = false, keyRight = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Rejestracja klasy okna
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "OpenGLWindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    // Tworzenie okna
    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        "OpenGL with WinAPI",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) {
        MessageBox(NULL, "Failed to create window.", "Error", MB_OK);
        return -1;
    }

    ShowWindow(hwnd, nCmdShow);

    // Ustawienie kontekstu OpenGL
    SetupOpenGL(hwnd);

    // Dopasowanie sceny do początkowych wymiarów okna
    RECT rect;
    GetClientRect(hwnd, &rect);
    ResizeScene(rect.right, rect.bottom);

    // Pętla zdarzeń
    MSG msg = {};
    HDC hdc = GetDC(hwnd);

    bool running = true;
    while (running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Obsługa ruchu kwadratu
        HandleMovement();
        MoveSquareToTarget();
        HandleCameraMovement();

        // Renderuj scenę w każdej iteracji
        RenderScene(hdc);
        Sleep(16); // ~60 FPS

    }

    // Czyszczenie zasobów
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hglrc);
    ReleaseDC(hwnd, hdc);

    return 0;
}

void ResizeScene(int width, int height) {
    if (height == 0) height = 1; // Zapobieganie dzieleniu przez zero

    glViewport(0, 0, width, height); // Dopasowanie widoku do nowych rozmiarów

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1); // Ustawienie układu współrzędnych dla całego okna

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void HandleMovement() {
    if (keyUp && squareY > 0) squareY -= squareSpeed;
    if (keyDown && squareY + squareSize < mapHeight) squareY += squareSpeed;
    if (keyLeft && squareX > 0) squareX -= squareSpeed;
    if (keyRight && squareX + squareSize < mapWidth) squareX += squareSpeed;
}

void MoveSquareToTarget() {
    if (targetX < 0 || targetY < 0) return; // Jeśli cel nie został ustawiony, nic nie rób

    // Oblicz różnicę pozycji
    float deltaX = targetX - squareX;
    float deltaY = targetY - squareY;

    // Oblicz dystans do celu
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

    // Jeśli kwadrat jest blisko celu, zatrzymaj go
    if (distance < squareSpeed) {
        squareX = targetX;
        squareY = targetY;
        targetX = -1; // Zresetuj cel
        targetY = -1;
        return;
    }

    // Oblicz normalizowany kierunek
    float dirX = deltaX / distance;
    float dirY = deltaY / distance;

    // Przesuń kwadrat w kierunku celu
    squareX += dirX * squareSpeed;
    squareY += dirY * squareSpeed;
}

void HandleCameraMovement() {
    // Przesuwaj kamerę, gdy mysz zbliża się do krawędzi
    int margin = 50; // Margines aktywujący przesunięcie

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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_MOUSEMOVE: {
            mouseX = LOWORD(lParam);
            mouseY = HIWORD(lParam);
            // HandleCameraMovement(mouseX, mouseY);
            return 0;
        }
        case WM_SIZE: {
            windowWidth = LOWORD(lParam);
            windowHeight = HIWORD(lParam);
            std::cout << "Width: " << windowWidth << ", Height: " << windowHeight << std::endl;
            ResizeScene(windowWidth, windowHeight);
            return 0;
            }
        case WM_KEYDOWN:  // Klawisz wciśnięty
            switch (wParam) {
                case VK_UP:
                    keyUp = true;
                    break;
                case VK_DOWN:
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
            }
            return 0;

        case WM_KEYUP:  // Klawisz zwolniony
            switch (wParam) {
                case VK_UP:
                    keyUp = false;
                    break;
                case VK_DOWN:
                    keyDown = false;
                    break;
                case VK_LEFT:
                    keyLeft = false;
                    break;
                case VK_RIGHT:
                    keyRight = false;
                    break;
                case VK_CONTROL:
                    transparency = 1.0f;
                    break;
            }
            return 0;

        case WM_LBUTTONDOWN: {
            // Pobierz współrzędne kliknięcia myszy
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);

            // Ustaw cel (odwróć Y, bo OpenGL ma początek w dolnym-lewym rogu)
            targetX = static_cast<float>(mouseX + cameraX);
            targetY = static_cast<float>(mouseY + cameraY);

            std::cout<<targetX<<" "<<targetY<<std::endl;

            return 0;
            }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void SetupOpenGL(HWND hwnd) {
    HDC hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    if (!SetPixelFormat(hdc, pixelFormat, &pfd)) {
        MessageBox(NULL, "Failed to set pixel format.", "Error", MB_OK);
        exit(-1);
    }

    hglrc = wglCreateContext(hdc);
    if (!hglrc) {
        MessageBox(NULL, "Failed to create OpenGL context.", "Error", MB_OK);
        exit(-1);
    }

    if (!wglMakeCurrent(hdc, hglrc)) {
        MessageBox(NULL, "Failed to activate OpenGL context.", "Error", MB_OK);
        exit(-1);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Ustawienia OpenGL
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Ustawienie perspektywy
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 1.0, 100.0);
}

void RenderScene(HDC hdc) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Renderowanie głównej sceny
    glPushMatrix();
    glTranslatef(-cameraX, -cameraY, 0.0f);

    // Rysowanie planszy i obiektów
    RenderMainScene();

    glPopMatrix();

    // Renderowanie minimapy
    RenderMiniMap();

    SwapBuffers(hdc);
}

void RenderMainScene() {
    // // Rysuj planszę (np. siatkę lub elementy)
    // for (float y = 110.0f; y < mapHeight; y += 550.0f) {
    //     for (float x = 110.0f; x < mapWidth; x += 550.0f) {
    //         float x1 = x;
    //         float y1 = y;
    //         float x2 = x + 55.0f;
    //         float y2 = y + 55.0f;

    //         glBegin(GL_QUADS);
    //         glColor4f(0.2f, 0.6f, 0.3f, 0.5f); // Dodano przezroczystość
    //         glVertex2f(x1, y1);
    //         glVertex2f(x2, y1);
    //         glVertex2f(x2, y2);
    //         glVertex2f(x1, y2);
    //         glEnd();
    //     }
    // }

    // Rysuj kwadrat gracza (czerwony, nieprzezroczysty)
    float x1 = squareX;
    float y1 = squareY;
    float x2 = squareX + squareSize;
    float y2 = squareY + squareSize;

    glBegin(GL_QUADS);
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f); // Nieprzezroczysty czerwony kwadrat
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();

    // Wyłącz test głębokości dla przezroczystości (opcjonalne)
    glDisable(GL_DEPTH_TEST);

    // Rysuj przezroczyste zielone kwadraty na wierzchu
    for (float y = 110.0f; y < mapHeight; y += 550.0f) {
        for (float x = 110.0f; x < mapWidth; x += 550.0f) {
            // Oblicz środek zielonego kwadratu
            float greenCenterX = x + 27.5f; // 27.5 = 55 / 2
            float greenCenterY = y + 27.5f;

            // Oblicz środek czerwonego kwadratu
            float redCenterX = squareX + squareSize / 2.0f;
            float redCenterY = squareY + squareSize / 2.0f;

            // Oblicz odległość między kwadratami
            float deltaX = greenCenterX - redCenterX;
            float deltaY = greenCenterY - redCenterY;
            float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

            // Ustaw przezroczystość na podstawie odległości
            float thresholdDistance = 200.0f; // Próg odległości
            float currentTransparency = (distance < thresholdDistance) ? 0.1f : 1.0f;

            // Rysuj zielony kwadrat z odpowiednią przezroczystością
            glBegin(GL_QUADS);
            glColor4f(0.2f, 0.6f, 0.3f, currentTransparency);
            glVertex2f(x, y);
            glVertex2f(x + 55.0f, y);
            glVertex2f(x + 55.0f, y + 55.0f);
            glVertex2f(x, y + 55.0f);
            glEnd();
        }
    }

    // Przywróć test głębokości
    glEnable(GL_DEPTH_TEST);
}

void RenderMiniMap() {
    // Ustaw widok dla minimapy
    glViewport(windowWidth - 200, windowHeight - 200, 200, 200); // Prawy górny róg
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, mapWidth, mapHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Rysuj kwadrat gracza na minimapie
    float miniSquareX = squareX;
    float miniSquareY = squareY;
    float miniSquareSize = squareSize;

    glBegin(GL_QUADS);
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f); // Kwadrat gracza
    glVertex2f(miniSquareX, miniSquareY);
    glVertex2f(miniSquareX + miniSquareSize, miniSquareY);
    glVertex2f(miniSquareX + miniSquareSize, miniSquareY + miniSquareSize);
    glVertex2f(miniSquareX, miniSquareY + miniSquareSize);
    glEnd();

    // Rysuj widok kamery
    float cameraViewX = cameraX;
    float cameraViewY = cameraY;
    float cameraViewWidth = windowWidth;
    float cameraViewHeight = windowHeight;

    glBegin(GL_LINE_LOOP);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Prostokąt widoku kamery
    glVertex2f(cameraViewX, cameraViewY);
    glVertex2f(cameraViewX + cameraViewWidth, cameraViewY);
    glVertex2f(cameraViewX + cameraViewWidth, cameraViewY + cameraViewHeight);
    glVertex2f(cameraViewX, cameraViewY + cameraViewHeight);
    glEnd();

    // Rysuj tło minimapy
    glBegin(GL_QUADS);
    glColor4f(0.1f, 0.3f, 0.5f, 1.0f); // Tło minimapy
    glVertex2f(0, 0);
    glVertex2f(mapWidth, 0);
    glVertex2f(mapWidth, mapHeight);
    glVertex2f(0, mapHeight);
    glEnd();

    glDisable(GL_DEPTH_TEST); // Wyłącz test głębokości
    //Rysuj obszar odkrywania zarośli przez gracza na minimapie
    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 1.0f, 0.01f);
    for(int i = 0; i < 360; i++){
        float x = squareX + 200 * cos(i);
        float y = squareY + 200 * sin(i);
        glVertex2f(x, y);
    }
    glEnd();

    glEnable(GL_DEPTH_TEST); // Włącz test głębokości

    // Przywróć widok do głównej sceny
    glViewport(0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);
}
