#include <windows.h>
#include <vector>

#include "../include/creta.h"
#include "../include/events.h"

std::vector<Event> Creta::getEvents() {
    Creta& creta = Creta::getInstance();
    if (!creta.initialized) exit(1);
    
    std::vector<Event> events;
	MSG msg;
    Event event;
	while (PeekMessage(&msg, creta.hwnd, 0, 0, PM_REMOVE)) {
        switch (msg.message) {
			case WM_KEYDOWN: {
				event.type = KEYDOWN;
				event.keyDown.key = msg.wParam;
			} break;

			case WM_KEYUP: {
				event.type = KEYUP;
				event.keyUp.key = msg.wParam;
			} break;

			case WM_MOUSEMOVE: {
				event.type = MOUSEMOTION;
				POINT pos;
				GetCursorPos(&pos);
				ScreenToClient(creta.hwnd, &pos);
				event.mouseMotion.prevPos = creta.mousePos;
				event.mouseMotion.pos = pos;
				creta.mousePos = pos;
			} break;

			default: {
				DispatchMessage(&msg);
			}
        }
		events.push_back(event);
	}
    return events;
}
