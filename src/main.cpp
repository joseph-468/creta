#include <windows.h>
#include <chrono>
#include <stdint.h>

#include "../include/creta.h"

// Creta framework testing
int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
    Creta::init();
    Creta::setTitle(L"Game Window");
    Creta::setSize(1280, 720);
    SpriteID img = Creta::loadSprite(L"assets/testsprite.bmp");

    bool moving = false;
    Rect rect = { 400, 400, 100, 200 };

    float deltaTime = 0.01666f;
    Clock clock;

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
            rect.y += 1;
        }

        Creta::clearScreen(GRAY);
        Creta::drawRect(rect, GREEN);
        Creta::drawSprite(img, 50, 50);
		Creta::render();
        deltaTime = clock.tick(60);
    }

    return EXIT_SUCCESS;
}
