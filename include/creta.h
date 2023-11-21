#pragma once
#include <vector>
#include <string>
#include <stdint.h>

#include "events.h"
#include "utility.h"

struct RenderState {
    int height, width;
    void *memory;
    BITMAPINFO bitmapInfo;
};

struct Rect {
	uint32_t x, y;
	uint32_t width, height;
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
	static void setSize(const uint32_t x, const uint32_t y);
	static void render();
	static std::vector<Event> getEvents();
	static void clearScreen(const uint32_t color);
    static void drawRect(const uint32_t left, const uint32_t top, const uint32_t right, const uint32_t bottom, const uint32_t color);
    static void drawRect(const Rect &rect, const uint32_t color);
	static SpriteID loadSprite(const std::wstring &filepath);
	static void drawSprite(const SpriteID spriteID, const uint32_t x, const uint32_t y);

private:
	bool initialized;
	HDC hdc;
	HWND hwnd;
	RenderState renderState;
	POINT mousePos;
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
