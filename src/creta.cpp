#include <windows.h>
#include <vector>
#include <string>
#include <iostream>

#include "../include/creta.h"
#include "../include/utility.h"

Creta::Creta() : initialized(false), hdc(nullptr), hwnd(nullptr), renderState(), mousePos(), spriteIDCounter(0), sprites() {}

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
            // Get new window size
            RECT rect;
            GetClientRect(hwnd, &rect);
            creta.renderState.width = rect.right - rect.left;
            creta.renderState.height = rect.bottom - rect.top;
            int bufferSize = creta.renderState.width * creta.renderState.height * sizeof(uint32_t);

            // Set renderstate to new window size
            if (creta.renderState.memory) VirtualFree(creta.renderState.memory, 0, MEM_RELEASE);
            creta.renderState.memory = VirtualAlloc(0, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            creta.renderState.bitmapInfo.bmiHeader.biSize = sizeof(creta.renderState.bitmapInfo.bmiHeader);
            creta.renderState.bitmapInfo.bmiHeader.biWidth = creta.renderState.width;
            creta.renderState.bitmapInfo.bmiHeader.biHeight = creta.renderState.height;
            creta.renderState.bitmapInfo.bmiHeader.biPlanes = 1;
            creta.renderState.bitmapInfo.bmiHeader.biBitCount = 32;
            creta.renderState.bitmapInfo.bmiHeader.biCompression = BI_RGB;

            // Set mouse position
            POINT pos;
            GetCursorPos(&pos);
            ScreenToClient(hwnd, &pos);
            creta.mousePos.x = pos.x;
            creta.mousePos.y = pos.y;
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

void Creta::setSize(const uint32_t width, const uint32_t height) {
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

void Creta::drawRect(const uint32_t left, const uint32_t top, const uint32_t right, const uint32_t bottom, const uint32_t color) {
    Creta& creta = Creta::getInstance();
    if (!creta.initialized) exit(1);

    uint32_t x0 = clamp(left, 0, creta.renderState.width);
    uint32_t x1 = clamp(right, 0, creta.renderState.width);
    uint32_t y0 = clamp(creta.renderState.height - top, 0, creta.renderState.height);
    uint32_t y1 = clamp(creta.renderState.height - bottom, 0, creta.renderState.height);
    for (uint32_t yPos = y1; yPos < y0; yPos++) {
        uint32_t *pixel = (uint32_t*)creta.renderState.memory + x0 + yPos * creta.renderState.width;
        for (uint32_t xPos = x0; xPos < x1; xPos++) {
            *pixel++ = color;
        }
    }
}

void Creta::drawRect(const Rect &rect, const uint32_t color) {
    Creta& creta = Creta::getInstance();
    if (!creta.initialized) exit(1);

    const uint32_t left = clamp(rect.x, 0, creta.renderState.width);
    const uint32_t right = clamp(rect.x + rect.width, 0, creta.renderState.width);
    const uint32_t top = clamp(creta.renderState.height - rect.y, 0, creta.renderState.height);
    const uint32_t bottom = clamp(creta.renderState.height - (rect.y + rect.height), 0, creta.renderState.height);
    for (uint32_t y = bottom; y < top; y++) {
        uint32_t *pixel = (uint32_t*)creta.renderState.memory + left + y * creta.renderState.width;
        for (uint32_t x = left; x < right; x++) {
            *pixel++ = color;
        }
    }
}

SpriteID Creta::loadSprite(const std::wstring& filepath, bool transparent) {
    Creta& creta = Creta::getInstance();
    if (!creta.initialized) exit(1);

    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, filepath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
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
    sprite.id = creta.spriteIDCounter++;
    sprite.transparent = transparent;
	sprite.size = bmp.bmWidth * bmp.bmHeight;
    if (transparent) sprite.size /= 2;
    sprite.width = bmp.bmWidth;
    sprite.height = bmp.bmHeight;
    sprite.data = pixels;
    creta.sprites[sprite.id] = sprite;
    DeleteObject(hBitmap);
    DeleteDC(hdc);
    return sprite.id;
}

void Creta::unloadSprite(const SpriteID spriteID) {
    Creta& creta = Creta::getInstance();
    if (!creta.initialized) exit(1);

    creta.sprites.erase(spriteID);
}

void Creta::drawSprite(const SpriteID spriteID, const uint32_t x, const uint32_t y) {
    Creta& creta = Creta::getInstance();
    if (!creta.initialized) exit(1);
    
    Sprite sprite = creta.sprites[spriteID];
    sprite.height /= (sprite.transparent) ? 2 : 1;
    uint32_t left = clamp(x, 0, creta.renderState.width);
    uint32_t right = clamp(x + sprite.width, 0, creta.renderState.width);
    uint32_t bottom = clamp(y + sprite.height, 0, creta.renderState.height);
    uint32_t top = clamp(y, 0, creta.renderState.height);

    for (uint32_t yPos = top; yPos < bottom; yPos++) {
        uint32_t spritePixel;
        uint32_t* pixel = (uint32_t*)creta.renderState.memory + left + ((creta.renderState.height - yPos-1) * creta.renderState.width);
        for (uint32_t xPos = left; xPos < right; xPos++) {
            spritePixel = xPos - x + (yPos - y) * sprite.width;
			if (!sprite.transparent || sprite.data[spritePixel + sprite.size] != BLACK)
				*pixel = sprite.data[spritePixel];
			pixel++;
        }
    }
}

Clock::Clock() {
	LARGE_INTEGER perf;
	QueryPerformanceFrequency(&perf);
    performanceFrequency = static_cast<float>(perf.QuadPart);
    QueryPerformanceCounter(&frameBeginTime);
    frameEndTime = frameBeginTime;
}

// Timing may be innacurate
float Clock::tick() {
	QueryPerformanceCounter(&frameEndTime);
	float deltaTime = static_cast<float>((frameEndTime.QuadPart - frameBeginTime.QuadPart)) / performanceFrequency;
	frameBeginTime = frameEndTime;
    return deltaTime;
}

float Clock::tick(const int FPS) {
	QueryPerformanceCounter(&frameEndTime);
	float deltaTime = static_cast<float>((frameEndTime.QuadPart - frameBeginTime.QuadPart)) / performanceFrequency;
    float sleepTime = (1000.0f / FPS) - (deltaTime * 1000.0f);
    if (sleepTime < 0) sleepTime = 0;
    Sleep(sleepTime);
    QueryPerformanceCounter(&frameEndTime);
	frameBeginTime = frameEndTime;
    return deltaTime + (sleepTime / 1000.0);
}
