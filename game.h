#pragma once

#ifndef game_h
#define game_h
#include "iostream"
#include "SDL.h"

class Game {
	bool run;
	SDL_Window* window;
	SDL_Renderer* renderer;

public:
	Game();
	~Game();

	void init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen);
	void handleEvents();
	void update();
	void render();
	void clean();

	bool running();
};


#endif // !game_h
