#pragma once
#ifndef texture_manager_h
#define texture_manager_h
#include <iostream>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

using namespace std;

class texture_manager {
	unordered_map<string, SDL_Texture*> textures;
	SDL_Renderer* renderer;
    SDL_Texture* missing_texture = nullptr;
public:
    texture_manager(SDL_Renderer* renderer);
    ~texture_manager();

    auto find_iter_by_name(const std::string& name) -> decltype(textures)::iterator;
    auto find_iter_by_name(const std::string& name) const -> decltype(textures)::const_iterator;
    SDL_Texture* load_texture(const string& name, const string& filename);
    SDL_Texture* get_texture(const string& name) const;
    void load_textures_from_folder(const string& folder_path);
    void set_renderer(SDL_Renderer* rend) { renderer = rend; }
    void unload_texture(const string& name);
    bool has(const string& name);
    void clear();
};

#endif