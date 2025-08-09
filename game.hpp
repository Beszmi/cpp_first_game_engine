#pragma once

#ifndef game_h
#define game_h
#include <iostream>
#include "SDL.h"
#include "SDL_image.h"
#include "texture_manager.hpp"
#include "game_obj.hpp"
#include "tilemap.hpp"

class Game {
	bool run;
	int cnt = 0;
	SDL_Window* window;
	SDL_Renderer* renderer;
	texture_manager tex_mgr;
	Game_obj_container obj_container;
	std::vector<std::unique_ptr<tilemap>> maps;
	int screen_w, screen_h;
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
