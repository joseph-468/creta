#pragma once

enum EventType {
	KEYDOWN,
	KEYUP,
	MOUSEBUTTONUP,
	MOUSEBUTTONDOWN,
	MOUSEMOTION,
	NONE,
};

enum MouseButton {
	LEFTBUTTON,
	MIDDLEBUTTON,
	RIGHTBUTTON,
	XBUTTON,
};

struct Event {
	EventType type;
	union {
		struct {
			uint32_t key;
		} keyDown;
		struct {
			uint32_t key;
		} keyUp;
		struct {
			POINT pos;
			uint32_t button;
		} mouseButtonUp;
		struct {
			POINT pos;
			uint32_t button;
		} mouseButtonDown;
		struct {
			POINT pos;
			POINT prevPos;
		} mouseMotion;
	};
};
