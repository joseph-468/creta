#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
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
	SpriteID id;
	uint32_t size; // Size of final image, doesn't include transparency mask
	uint32_t width, height;
	uint32_t *data;
	bool transparent;
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
	static SpriteID loadSprite(const std::wstring &filepath, bool transparent);
	static void unloadSprite(const SpriteID spriteID);
	static void drawSprite(const SpriteID spriteID, const uint32_t x, const uint32_t y);

private:
	bool initialized;
	HDC hdc;
	HWND hwnd;
	RenderState renderState;
	POINT mousePos;
	SpriteID spriteIDCounter;
	std::unordered_map<SpriteID, Sprite> sprites;

	Creta();
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

inline SpriteID Creta::loadSprite(const std::wstring& filepath) {
    return Creta::loadSprite(filepath, false);
}

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
