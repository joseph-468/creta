#include <windows.h>
#include <chrono>
#include <stdint.h>

#include "../include/creta.h"

// Creta framework testing
int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
    Creta::init();
    Creta::setTitle(L"Game Window");
    Creta::setSize(1280, 720);
    SpriteID sprite1 = Creta::loadSprite(L"assets/testsprite.bmp");

    float deltaTime = 0.01666f;
    Clock clock;

    bool moving = false;
    int x = 200;
	int y = 100;

    while (true) {
        for (MSG &msg : Creta::getEvents()) {
            if (msg.message == WM_KEYDOWN) {
                if (msg.wParam == VK_DOWN) {
                    moving = true;
                }
            }
            if (msg.message == WM_KEYUP) {
                if (msg.wParam == VK_DOWN) {
					moving = false;
                }
            }
        }
        if (moving) {
            y += 1;
        }

        Creta::clearScreen(GRAY);
        Creta::drawRect(400, 400, 500, 600, GREEN);
        Creta::drawSprite(sprite1, x, y);
		Creta::render();
        deltaTime = clock.tick(60);
    }

    return EXIT_SUCCESS;
}
