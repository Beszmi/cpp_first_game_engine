#pragma once
#ifndef texture_manager_h
#define texture_manager_h
#include <iostream>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <unordered_map>
using namespace std;


class texture_manager {
	unordered_map<std::string, SDL_Texture*> textures;
	SDL_Renderer* renderer;
    SDL_Texture* missing_texture = nullptr;
public:
    texture_manager(SDL_Renderer* renderer);
    ~texture_manager();

    SDL_Texture* load_texture(const std::string& name, const std::string& filename);
    SDL_Texture* get_texture(const std::string& name) const;
    void load_textures_from_folder(const std::string& folder_path);
    void set_renderer(SDL_Renderer* rend) { renderer = rend; }
    void unload_texture(const std::string& name);
    void clear();
};



#endif