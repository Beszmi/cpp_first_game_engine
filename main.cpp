#include "SDL.h"
#include "game.hpp"

int main(int argc, char *argv[]) {
	const int fps_max = 120;
	const int framedelay = 1000 / fps_max;
	Uint32 framestart;
	int frametime;

	Game* game1 = new Game();
	game1->init("epic game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, false);
	TTF_Init();

	while (game1->running()) {
		framestart = SDL_GetTicks();
		game1->handleEvents();
		game1->update();
		game1->render();

		frametime = SDL_GetTicks() - framestart;

		if (framedelay > frametime) {
			SDL_Delay(framedelay - frametime);
		}
	}
	game1->clean();

	return 0;
}