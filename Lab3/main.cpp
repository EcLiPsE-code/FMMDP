#include <iostream>
#include <chrono>
#include <sstream>
#include <Windows.h>

#include "Timer.h"
#include "common.h"

Timer calculate();

struct ThreadArgument {
    double a, b, eps;
    int n, totalPoints;
};

DWORD WINAPI threadFunc(LPVOID arg);

void drawAxis(HDC hdc, RECT rect);

void drawGraph(HDC hdc, RECT rectClient);

void onPaint(HWND hwnd);

LONG WINAPI wndProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam);

int showWindow(HINSTANCE &hInstance, int nShowCmd);

uint32_t threadsCount = 3;
double a = -3, b = 3, eps = 0.01;
int totalPoints = 50000;
int n = 1000;
double calculatedVolume;

HANDLE mutex;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "Количество потоков: " << threadsCount << std::endl;
    std::cout << "Начало диапазона (a): " << a << std::endl;
    std::cout << "Конец диапазона (b): " << b << std::endl;
    std::cout << "Требуемая точность (eps): " << eps << std::endl << std::endl;
    std::cout << "Общее кол-во точек (totalPoints): " << totalPoints << std::endl;
    std::cout << "n: " << n << std::endl;

    Timer timer = calculate();

    std::cout << "Затраченное время (в миллисекундах): " << timer.elapsedMilliseconds() << std::endl;
    std::cout << "Вычисленная площадь: " << calculatedVolume << std::endl;

    return showWindow(hInstance, nShowCmd);
}

Timer calculate() {
    Timer timer;
    calculatedVolume = 0.0;
    mutex = CreateMutex(nullptr, FALSE, "calculatedVolume");

    double delta = (b - a) / threadsCount;
    auto *threads = new HANDLE[threadsCount];
    for (uint32_t i = 0; i < threadsCount; ++i) {
        double start = a + (delta * i);
        threads[i] = CreateThread(nullptr, 4 * 1024, &threadFunc,
                                  new ThreadArgument{start, start + delta, eps,
                                                     n, totalPoints},
                                  0, nullptr);
    }
    WaitForMultipleObjects(threadsCount, threads, TRUE, INFINITE);
    for (uint32_t i = 0; i < threadsCount; ++i) {
        CloseHandle(threads[i]);
    }
    CloseHandle(mutex);
    timer.stop();
    return timer;
}

DWORD WINAPI threadFunc(LPVOID arg) {
    auto *argument = static_cast<ThreadArgument *>(arg);
    double result = methodSimpson(argument->a, argument->b, argument->eps, argument->n, argument->totalPoints);
    delete argument;
    WaitForSingleObject(mutex, INFINITE);
    calculatedVolume += result;
    ReleaseMutex(mutex);
    return 0;
}

void drawAxis(HDC hdc, RECT rect) {
    HPEN penGraph = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    HGDIOBJ gdiOld = SelectObject(hdc, penGraph);
    MoveToEx(hdc, 0, rect.bottom / 2, nullptr);
    LineTo(hdc, rect.right, rect.bottom / 2);
    LineTo(hdc, rect.right - 5, rect.bottom / 2 + 2);
    MoveToEx(hdc, rect.right, rect.bottom / 2, nullptr);
    LineTo(hdc, rect.right - 5, rect.bottom / 2 - 2);
    MoveToEx(hdc, rect.right / 2, rect.bottom, nullptr);
    LineTo(hdc, rect.right / 2, rect.top);
    LineTo(hdc, rect.right / 2 - 2, rect.top + 5);
    MoveToEx(hdc, rect.right / 2, rect.top, nullptr);
    LineTo(hdc, rect.right / 2 + 2, rect.top + 5);
    SelectObject(hdc, gdiOld);
}

void drawGraph(HDC hdc, RECT rectClient) {
    HPEN penGraph = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    HGDIOBJ gdiOld = SelectObject(hdc, penGraph);
    double x = a;
    double step = (b - a) / rectClient.right;
    double yA = function(a);
    MoveToEx(hdc, 0, int(-yA / step) + rectClient.bottom / 2, nullptr);
    while (x < b) {
        x += step;
        double y = function(x);
        LineTo(hdc, int(x / step) + rectClient.right / 2, int(-y / step) + rectClient.bottom / 2);
    }
    SelectObject(hdc, gdiOld);
}

void onPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    RECT rectClient;
    HDC hdc = BeginPaint(hwnd, &ps);
    GetClientRect(hwnd, &rectClient);
    drawAxis(hdc, rectClient);
    drawGraph(hdc, rectClient);
    ValidateRect(hwnd, nullptr);
    EndPaint(hwnd, &ps);
}

LONG wndProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam) {
    switch (Message) {
        case WM_PAINT:
            onPaint(hwnd);
            break;
        case WM_MOUSEMOVE:
            SetCapture(hwnd);
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
            ReleaseCapture();
            break;
        case WM_WINDOWPOSCHANGED:
            UpdateWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, Message, wparam, lparam);
    }
    UpdateWindow(hwnd);
    return 0;
}

int showWindow(HINSTANCE &hInstance, int nShowCmd) {
    WNDCLASS w = {
            .style = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc = reinterpret_cast<WNDPROC>(wndProc),
            .hInstance = hInstance,
            .hbrBackground = CreateSolidBrush(0x00FFFFFF),
            .lpszClassName = "Window"
    };
    RegisterClass(&w);
    HWND hwnd = CreateWindowExW(0, L"Window", L"Лабораторная работа №3 График функции",
                                WS_OVERLAPPEDWINDOW,
                                400, 300, 600, 400,
                                nullptr, nullptr, hInstance, nullptr);
    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
