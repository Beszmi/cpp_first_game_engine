#pragma once

#ifndef game_h
#define game_h
#include <iostream>
#include "SDL.h"
#include "SDL_image.h"
#include "graphic_components/texture_manager.hpp"
#include "game_obj.hpp"
#include "graphic_components/tilemap.hpp"
#include "graphic_components/camera.hpp"

class Game {
	bool run;
	//int cnt = 0;
	SDL_Window* window;
	SDL_Renderer* renderer;
	texture_manager tex_mgr;
	Game_obj_container obj_container;
	std::vector<std::unique_ptr<tilemap>> maps;
	int screen_w, screen_h;
	Camera cam;
public:
	Game();
	~Game();

	void init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen);
	void handleEvents();
	void update(double dtSeconds);
	void render();
	void clean();

	bool running() const;
};


#endif // !game_h
