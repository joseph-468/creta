#pragma once
#include <vector>
#include <string>
#include <stdint.h>

#include "utility.h"

struct RenderState {
    int height, width;
    void *memory;
    BITMAPINFO bitmapInfo;
};

using SpriteID = uint32_t;

struct Sprite {
	uint32_t width, height;
	uint32_t *data;
};

class Creta {
public:
	Creta(const Creta&) = delete;
	static Creta& getInstance();
	static void init();
	static void setTitle(const std::wstring &name);
	static void setSize(const uint16_t x, const uint16_t y);
	static void render();
	static std::vector<MSG> getEvents();
	static void clearScreen(const uint32_t color);
    static void drawRect(int x0, int y0, int x1, int y1, uint32_t color);
	static SpriteID loadSprite(const std::wstring &filepath);
	static void drawSprite(const SpriteID spriteID, int x, int y);

private:
	bool initialized;
	HDC hdc;
	HWND hwnd;
	RenderState renderState;
	std::vector<Sprite> sprites;

	Creta();
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

class Clock {
public:
	Clock();
	float tick();
	float tick(const int FPS);

private:
	float performanceFrequency;
    LARGE_INTEGER frameBeginTime;
    LARGE_INTEGER frameEndTime;
};
