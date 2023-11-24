#include <windows.h>
#include <chrono>
#include <stdint.h>

#include "../include/creta.h"

// Creta framework testing
int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
    Creta::init();
    Creta::setTitle(L"Game Window");
    Creta::setSize(1280, 720);

    SpriteID img = Creta::loadSprite(L"assets/transparentsprite.bmp", true);
    Rect rect = { 200, 200, 50, 50 };

    PlaySound(L"assets/music.wav", NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);

    float deltaTime = 0.01666f;
    Clock clock;

    while (true) {
        for (Event &event: Creta::getEvents()) {
            if (event.type == MOUSEMOTION) {
                rect.x = event.mouseMotion.pos.x;
                rect.y = event.mouseMotion.pos.y;
            }
        }

        Creta::clearScreen(GRAY);
        Creta::drawRect(rect, BLUE);
        Creta::drawSprite(img, 400, 400);
		Creta::render();
        deltaTime = clock.tick(60);
    }

    return EXIT_SUCCESS;
}
