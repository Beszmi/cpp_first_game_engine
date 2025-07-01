#pragma once

#ifndef game_obj_hpp
#define game_obj_hpp
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include "texture_manager.hpp"

class GameObject {
	std::string name;
	int xpos, ypos;
	SDL_Texture* obj_tex;
	SDL_Rect src_rect, dst_rect;
	SDL_Renderer* renderer;
public:
	GameObject(const std::string& name, const std::string& texture, SDL_Renderer* ren, texture_manager& tex_mgr);

	const std::string& get_name() const { return name; }
	void set_position(int x, int y) { xpos = x; ypos = y; }

	void update();
	void render();
};

class Game_obj_container {
	std::unordered_map<std::string, std::unique_ptr<GameObject>> objects;
public:
	void spawn(const std::string& name, const std::string& texName, SDL_Renderer* ren, texture_manager& tm);
	void spawn(const std::string& texName, SDL_Renderer* ren, texture_manager& tm);
	GameObject* get(const std::string& name);
	void update_all();
	void render_all();
};

#endif