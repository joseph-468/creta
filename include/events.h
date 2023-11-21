#pragma once

enum EventType {
	KEYDOWN,
	KEYUP,
	MOUSEBUTTONUP,
	MOUSEBUTTONDOWN,
	MOUSEMOTION,
};

// Need to add key modifier member to key up and key down

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
		} mouseButtonUp;
		struct {
			POINT pos;
		} mouseButtonDown;
		struct {
			POINT pos;
			POINT prevPos;
		} mouseMotion;
	};
};
