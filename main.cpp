#include "SDL.h"
#include "game.hpp"

int main(int argc, char *argv[]) {
	const int fps_max = 120;
	const double target_dt = 1.0 / fps_max;

	Game* game1 = new Game();
	game1->init("epic game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, false);

	Uint64 now = SDL_GetPerformanceCounter();
	Uint64 last = now;
	const double freq = static_cast<double>(SDL_GetPerformanceFrequency());
	const double max_dt = 0.25;

	while (game1->running()) {
		now = SDL_GetPerformanceCounter();
		double dt = static_cast<double>(now - last) / freq;
		last = now;

		if (dt > max_dt) dt = max_dt;

		game1->handleEvents();
		game1->update(dt);
		game1->render();

		double frame_time = static_cast<double>(SDL_GetPerformanceCounter() - now) / freq;
		if (frame_time < target_dt) {
			Uint32 ms = static_cast<Uint32>((target_dt - frame_time) * 1000.0);
			if (ms > 0) SDL_Delay(ms);
		}
	}
	game1->clean();

	return 0;
}