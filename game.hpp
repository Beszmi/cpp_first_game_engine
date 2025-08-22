#pragma once

#ifndef game_h
#define game_h
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "graphic_components/texture_manager.hpp"
#include "game_obj.hpp"
#include "graphic_components/tilemap.hpp"
#include "graphic_components/camera.hpp"

struct mouse_state {
	float x = 0, y = 0;
	float click_x = 0, click_y = 0;
	float dx = 0, dy = 0;
	SDL_MouseButtonFlags held = 0;
	bool pressed[6] = {};
	bool released[6] = {};
	Uint8 clicks[6] = {};
};

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
	mouse_state mouse;
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
