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
		long double sf_frametime;
		if (frametime == 0) {
			sf_frametime = 999;
		}
		else {
			sf_frametime = frametime;
		}
		long double fps_jelenleg = 1000 / sf_frametime;
		std::cout << fps_jelenleg << std::endl;

		if (framedelay > frametime) {
			SDL_Delay(framedelay - frametime);
		}
	}
	game1->clean();

	//SDL_Delay(3000);
	return 0;
}