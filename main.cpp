#include "SDL.h"
#include "game.h"

int main(int argc, char *argv[]) {
	Game* game1 = new Game();
	game1->init("epic game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, true);

	while (game1->running()) {
		game1->handleEvents();
		game1->update();
		game1->render();
	}
	game1->clean();

	//SDL_Delay(3000);

	return 0;
}