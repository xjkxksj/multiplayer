#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

// Pozycje kwadratów
struct Position {
    int x, y;
};

Position mySquare = {300, 300};    // Kwadrat sterowany przez klienta
Position remoteSquare = {100, 100}; // Kwadrat sterowany przez serwer

// Rysowanie w oknie
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

            // Rysuj kwadraty
            Rectangle(hdc, mySquare.x, mySquare.y, mySquare.x + 50, mySquare.y + 50);  // Nasz kwadrat
            Rectangle(hdc, remoteSquare.x, remoteSquare.y, remoteSquare.x + 50, remoteSquare.y + 50);  // Kwadrat serwera

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_KEYDOWN: {
            // Ruch naszego kwadratu
            switch (wParam) {
                case VK_UP:    mySquare.y -= 5; break;
                case VK_DOWN:  mySquare.y += 5; break;
                case VK_LEFT:  mySquare.x -= 5; break;
                case VK_RIGHT: mySquare.x += 5; break;
            }

            // Odśwież okno
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void NetworkHandler(SOCKET socket, HWND hwnd, bool isServer) {
    Position lastMySquare = mySquare;          // Ostatnia znana pozycja lokalnego kwadratu
    Position lastRemoteSquare = remoteSquare; // Ostatnia znana pozycja zdalnego kwadratu

    while (true) {
        if (isServer) {
            // Serwer: odbiera od klienta, wysyła swoje dane
            recv(socket, (char*)&remoteSquare, sizeof(remoteSquare), 0);
            send(socket, (char*)&mySquare, sizeof(mySquare), 0);
        } else {
            // Klient: wysyła swoje dane, odbiera od serwera
            send(socket, (char*)&mySquare, sizeof(mySquare), 0);
            recv(socket, (char*)&remoteSquare, sizeof(remoteSquare), 0);
        }

        // Sprawdź, czy nastąpiła zmiana pozycji lokalnego lub zdalnego kwadratu
        bool mySquareChanged = (mySquare.x != lastMySquare.x || mySquare.y != lastMySquare.y);
        bool remoteSquareChanged = (remoteSquare.x != lastRemoteSquare.x || remoteSquare.y != lastRemoteSquare.y);

        if (mySquareChanged || remoteSquareChanged) {
            // Odśwież okno tylko, jeśli któryś kwadrat zmienił pozycję
            InvalidateRect(hwnd, NULL, TRUE);

            // Zaktualizuj ostatnie znane pozycje
            lastMySquare = mySquare;
            lastRemoteSquare = remoteSquare;
        }
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    WSADATA wsa;
    SOCKET clientSocket;
    sockaddr_in serverAddr;

    // Inicjalizacja Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "Failed to initialize Winsock. Error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // Tworzenie gniazda klienta
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Konfiguracja adresu serwera
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("192.168.1.28");  // Adres lokalny serwera
    serverAddr.sin_port = htons(8080);  // Port serwera

    // Połączenie z serwerem
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection to server failed. Error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to the server!" << std::endl;

    // Tworzenie okna
    const char CLASS_NAME[] = "ClientWindow";
    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Klient - Steruj kwadratem strzałkami",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    ShowWindow(hwnd, SW_SHOW);

    // Uruchom wątek do obsługi sieci
    std::thread networkThread(NetworkHandler, clientSocket, hwnd, false);

    // Pętla komunikatów
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    networkThread.join();  // Poczekaj na zakończenie wątku sieciowego
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
