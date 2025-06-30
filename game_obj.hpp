#pragma once

#ifndef game_obj_h
#define game_obj_h
#include <iostream>
#include "SDL.h"
#include "SDL_image.h"
#include "texture_manager.hpp"

class GameObject {
	int xpos, ypos;
	SDL_Texture* obj_tex;
	SDL_Rect src_rect, dst_rect;
	SDL_Renderer* renderer;
public:
	GameObject(const string texture, SDL_Renderer* ren, texture_manager* tex_mgr);

	void update();
	void render();

	~GameObject();
};

#endif