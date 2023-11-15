#include <windows.h>
#include <vector>
#include <string>
#include <iostream>

#include "../include/creta.h"
#include "../include/utility.h"

Creta::Creta() : initialized(false), hdc(nullptr), hwnd(nullptr), renderState() {}

Creta& Creta::getInstance() {
    static Creta instance;
    return instance;
}

void Creta::init() {
    Creta& creta = Creta::getInstance();
    creta.initialized = true;

    // Window setup
    WNDCLASS wndClass = {};
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpszClassName = L"Creta Game Window";
    wndClass.lpfnWndProc = Creta::WindowProc; 
    RegisterClass(&wndClass);
    HINSTANCE hInstance = GetModuleHandle(NULL);
    creta.hwnd = CreateWindow(wndClass.lpszClassName, L"Creta Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 800, 800, 0, 0, hInstance, 0);
    creta.hdc = GetDC(creta.hwnd);
}

LRESULT CALLBACK Creta::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Creta& creta = Creta::getInstance();
    LRESULT result = 0;
    switch (uMsg) {
        // Handle window closing
        case WM_CLOSE:
        case WM_DESTROY: {
            PostQuitMessage(0);
            exit(0);
        } break;

        // Handle window resize
        case WM_SIZE: {
            RECT rect;
            GetClientRect(hwnd, &rect);
            creta.renderState.width = rect.right - rect.left;
            creta.renderState.height = rect.bottom - rect.top;
            int bufferSize = creta.renderState.width * creta.renderState.height * sizeof(uint32_t);

            if (creta.renderState.memory) VirtualFree(creta.renderState.memory, 0, MEM_RELEASE);
            creta.renderState.memory = VirtualAlloc(0, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            creta.renderState.bitmapInfo.bmiHeader.biSize = sizeof(creta.renderState.bitmapInfo.bmiHeader);
            creta.renderState.bitmapInfo.bmiHeader.biWidth = creta.renderState.width;
            creta.renderState.bitmapInfo.bmiHeader.biHeight = creta.renderState.height;
            creta.renderState.bitmapInfo.bmiHeader.biPlanes = 1;
            creta.renderState.bitmapInfo.bmiHeader.biBitCount = 32;
            creta.renderState.bitmapInfo.bmiHeader.biCompression = BI_RGB;
        } break;

        default: {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    return result; 
}

void Creta::setTitle(const std::wstring &windowTitle) {
    Creta& creta = Creta::getInstance();
    SetWindowText(creta.hwnd, windowTitle.c_str());
}

void Creta::setSize(const uint16_t width, const uint16_t height) {
    Creta& creta = Creta::getInstance();
    if (!creta.initialized) exit(1);

    SetWindowPos(creta.hwnd, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER);
}

void Creta::render() {
    Creta& creta = Creta::getInstance();
    if (!creta.initialized) exit(1);

	StretchDIBits(creta.hdc, 0, 0, creta.renderState.width, creta.renderState.height, 0, 0,
        creta.renderState.width, creta.renderState.height, creta.renderState.memory, &creta.renderState.bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

std::vector<MSG> Creta::getEvents() {
    Creta& creta = Creta::getInstance();
    if (!creta.initialized) exit(1);
    
    std::vector<MSG> events;
	MSG message;
	while (PeekMessage(&message, creta.hwnd, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
        DispatchMessage(&message);
        events.push_back(message);
	}
    return events;
}

void Creta::clearScreen(const uint32_t color) {
    Creta& creta = Creta::getInstance();
    if (!creta.initialized) exit(1);

	uint32_t *pixel = (uint32_t*)creta.renderState.memory;
	for (int y = 0; y < creta.renderState.height; y++) {
		for (int x = 0; x < creta.renderState.width; x++) {
			*pixel++ = color;
		}
	}
}

void Creta::drawRect(int x0, int y0, int x1, int y1, const uint32_t color) {
    Creta& creta = Creta::getInstance();
    if (!creta.initialized) exit(1);

    y0 = creta.renderState.height - y0;
    y1 = creta.renderState.height - y1;
    x0 = clamp(x0, 0, creta.renderState.width);
    x1 = clamp(x1, 0, creta.renderState.width);
    y0 = clamp(y0, 0, creta.renderState.height);
    y1 = clamp(y1, 0, creta.renderState.height);
    for (int y = y1; y < y0; y++) {
        uint32_t *pixel = (uint32_t*)creta.renderState.memory + x0 + y * creta.renderState.width;
        for (int x = x0; x < x1; x++) {
            *pixel++ = color;
        }
    }
}

SpriteID Creta::loadSprite(const std::wstring& filepath) {
    Creta& creta = Creta::getInstance();
    if (!creta.initialized) exit(1);

    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, filepath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    HDC hdc = CreateCompatibleDC(nullptr);
    SelectObject(hdc, hBitmap);
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    uint32_t* pixels = new uint32_t[bmp.bmWidth * bmp.bmHeight];
	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = bmp.bmWidth;
	bmi.bmiHeader.biHeight = -bmp.bmHeight;
	bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	GetDIBits(hdc, hBitmap, 0, bmp.bmHeight, pixels, &bmi, DIB_RGB_COLORS);

    Sprite sprite;
    sprite.width = bmp.bmWidth;
    sprite.height = bmp.bmHeight;
    sprite.data = pixels;
    creta.sprites.push_back(sprite);
    DeleteObject(hBitmap);
    DeleteDC(hdc);
    return creta.sprites.size()-1;
}

void Creta::drawSprite(const SpriteID spriteID, int xPos, int yPos) {
    Creta& creta = Creta::getInstance();
    if (!creta.initialized) exit(1);
    
    Sprite sprite = creta.sprites[spriteID];
    uint32_t left = clamp(xPos, 0, creta.renderState.width);
    uint32_t right = clamp(xPos + sprite.width, 0, creta.renderState.width);
    uint32_t bottom = clamp(yPos + sprite.height, 0, creta.renderState.height);
    uint32_t top = clamp(yPos, 0, creta.renderState.height);

    for (int y = top; y < bottom; y++) {
        uint32_t* pixel = (uint32_t*)creta.renderState.memory + left + ((creta.renderState.height - y-1) * creta.renderState.width);
        for (int x = left; x < right; x++) {
            *pixel++ = (uint32_t)sprite.data[x-xPos + (y-yPos) * sprite.width];
        }
    }
}

Clock::Clock() {
	LARGE_INTEGER perf;
	QueryPerformanceFrequency(&perf);
	Clock::performanceFrequency = perf.QuadPart;
    QueryPerformanceCounter(&frameBeginTime);
}

// Timing may be innacurate
float Clock::tick() {
	QueryPerformanceCounter(&frameEndTime);
	float deltaTime = static_cast<float>((Clock::frameEndTime.QuadPart - Clock::frameBeginTime.QuadPart)) / Clock::performanceFrequency;
	Clock::frameBeginTime = Clock::frameEndTime;
    return deltaTime;
}

float Clock::tick(const int FPS) {
	QueryPerformanceCounter(&frameEndTime);
	float deltaTime = static_cast<float>((Clock::frameEndTime.QuadPart - Clock::frameBeginTime.QuadPart)) / Clock::performanceFrequency;
    float sleepTime = (1000.0 / FPS) - (deltaTime * 1000.0);
    if (sleepTime < 0) sleepTime = 0;
    Sleep(sleepTime);
    QueryPerformanceCounter(&frameEndTime);
	Clock::frameBeginTime = Clock::frameEndTime;
    return deltaTime + (sleepTime / 1000.0);
}
