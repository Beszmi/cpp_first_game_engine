#pragma once

#ifndef game_h
#define game_h
#include <iostream>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

class Game {
	bool run;
	int cnt = 0;
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
