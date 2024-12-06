#include <windows.h>

// Rozmiar prostokąta
const int rectWidth = 150;
const int rectHeight = 100;

// Współrzędne prostokąta (zawsze w prawym dolnym rogu)
int rectX, rectY;
int windowWidth, windowHeight;

// Stany klawiszy
bool keyUp = false, keyDown = false, keyLeft = false, keyRight = false;

// Funkcja obsługi zdarzeń dla nowego okna
LRESULT CALLBACK ChildWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            // Czyszczenie tła
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            // Tekst w nowym oknie
            TextOut(hdc, 10, 10, "To jest nowe okno!", 18);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Funkcja obsługi zdarzeń okna
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_LBUTTONDOWN: {  // Obsługa kliknięcia lewym przyciskiem myszy
            const char CHILD_CLASS_NAME[] = "ChildWindow";

            // Rejestracja klasy nowego okna
            WNDCLASS wc = {};
            wc.lpfnWndProc = ChildWindowProc;  // Procedura obsługi nowego okna
            wc.hInstance = GetModuleHandle(NULL);
            wc.lpszClassName = CHILD_CLASS_NAME;

            RegisterClass(&wc);

            // Tworzenie nowego okna
            HWND childHwnd = CreateWindowEx(
                0,
                CHILD_CLASS_NAME,
                "Nowe Okno",
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
                NULL, NULL, GetModuleHandle(NULL), NULL
            );

            if (childHwnd != NULL) {
                ShowWindow(childHwnd, SW_SHOW);
            }

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
            }
            return 0;

        case WM_SIZE: {  // Obsługa zmiany rozmiaru okna
            windowWidth = LOWORD(lParam);  // Szerokość okna
            windowHeight = HIWORD(lParam); // Wysokość okna

            // Obliczanie nowej pozycji prostokąta
            rectX = windowWidth - rectWidth - 10;  // 10-pikselowy margines
            rectY = windowHeight - rectHeight - 10;

            // Wymuszenie odświeżenia okna
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        case WM_PAINT: {  // Rysowanie w oknie
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            // Czyszczenie tła
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            // Rysowanie prostokąta
            Rectangle(hdc, rectX, rectY, rectX + rectWidth, rectY + rectHeight);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Aktualizacja pozycji prostokąta na podstawie stanu klawiszy
void UpdatePosition() {
    int speed = 5;  // Prędkość ruchu
    if (keyUp) rectY -= speed;
    if (keyDown) rectY += speed;
    if (keyLeft) rectX -= speed;
    if (keyRight) rectX += speed;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "MojeOkno";

    // Rejestracja klasy okna
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Tworzenie okna
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Przesuwanie Prostokąta po Skosie",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Pętla komunikatów
    MSG msg = {};
    while (true) {
        // Obsługa komunikatów
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return 0;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Aktualizacja pozycji prostokąta
        UpdatePosition();

        // Wymuszenie odświeżenia okna
        InvalidateRect(hwnd, NULL, TRUE);

        // Opóźnienie (60 FPS)
        Sleep(5);
    }

    return 0;
}
