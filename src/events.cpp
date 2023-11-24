#include <windows.h>
#include <vector>

#include "../include/creta.h"
#include "../include/events.h"

std::vector<Event> Creta::getEvents() {
    Creta& creta = Creta::getInstance();
    if (!creta.initialized) exit(1);

	static uint8_t keys[256] = {};

	std::vector<Event> events(16);
	MSG msg;
	Event event = {};
	while (PeekMessage(&msg, creta.hwnd, 0, 0, PM_REMOVE)) {
        switch (msg.message) {
			// Repeats because of windows being stupid, check before sending event
			case WM_KEYDOWN: {
				event.type = KEYDOWN;
				event.keyDown.key = msg.wParam;
			} break;

			case WM_KEYUP: {
				event.type = KEYUP;
				event.keyUp.key = msg.wParam;
				keys[msg.wParam] = 0;
			} break;

			case WM_MOUSEMOVE: {
				event.type = MOUSEMOTION;
			} break;

			// Mouse clicks
			case WM_LBUTTONDOWN: {
				event.type = MOUSEBUTTONDOWN;
				event.mouseButtonDown.button = LEFTBUTTON;
				event.mouseButtonDown.pos = creta.mousePos;
			} break;

			case WM_LBUTTONUP: {
				event.type = MOUSEBUTTONUP;
				event.mouseButtonUp.button = LEFTBUTTON;
				event.mouseButtonUp.pos = creta.mousePos;
			} break;

			case WM_MBUTTONDOWN: {
				event.type = MOUSEBUTTONDOWN;
				event.mouseButtonDown.button = MIDDLEBUTTON;
				event.mouseButtonDown.pos = creta.mousePos;
			} break;

			case WM_MBUTTONUP: {
				event.type = MOUSEBUTTONUP;
				event.mouseButtonUp.button = MIDDLEBUTTON;
				event.mouseButtonUp.pos = creta.mousePos;
			} break;

			case WM_RBUTTONDOWN: {
				event.type = MOUSEBUTTONDOWN;
				event.mouseButtonDown.button = RIGHTBUTTON;
				event.mouseButtonDown.pos = creta.mousePos;
			} break;

			case WM_RBUTTONUP: {
				event.type = MOUSEBUTTONUP;
				event.mouseButtonUp.button = RIGHTBUTTON;
				event.mouseButtonUp.pos = creta.mousePos;
			} break;

			case WM_XBUTTONDOWN: {
				event.type = MOUSEBUTTONDOWN;
				event.mouseButtonDown.button = XBUTTON;
				event.mouseButtonDown.pos = creta.mousePos;
			} break;

			case WM_XBUTTONUP: {
				event.type = MOUSEBUTTONUP;
				event.mouseButtonUp.button = XBUTTON;
				event.mouseButtonUp.pos = creta.mousePos;
			} break;

			default: {
				event.type = NONE;
				DispatchMessage(&msg);
			}
        }
		// Mouse motion event is sent regardless of motion for some reason 
		if (event.type == MOUSEMOTION) {
			POINT pos;
			GetCursorPos(&pos);
			ScreenToClient(creta.hwnd, &pos);
			if (creta.mousePos.x != pos.x || creta.mousePos.y != pos.y) {
				event.mouseMotion.prevPos = creta.mousePos;
				event.mouseMotion.pos = pos;
				creta.mousePos = pos;
				events.push_back(event);
			}
		}
		else if (event.type == KEYDOWN) {
			if (!keys[event.keyDown.key]) events.push_back(event);
			keys[event.keyDown.key] = 1;
		}
		else if (event.type != NONE) {
			events.push_back(event);
		}
	}
    return events;
}
