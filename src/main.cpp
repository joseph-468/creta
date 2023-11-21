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

    int x = 400;
    int y = 400;

    float deltaTime = 0.01666f;
    Clock clock;

    while (true) {
        for (Event &event: Creta::getEvents()) {
            if (event.type == MOUSEMOTION) {
                x = event.mouseMotion.pos.x;
                y = event.mouseMotion.pos.y;
            }
        }

        Creta::clearScreen(GRAY);
        Creta::drawSprite(img, x, y);
		Creta::render();
        deltaTime = clock.tick(60);
    }

    return EXIT_SUCCESS;
}
