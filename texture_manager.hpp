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
	unordered_map<string, SDL_Texture*> textures;
	SDL_Renderer* renderer;
    SDL_Texture* missing_texture = nullptr;
public:
    texture_manager(SDL_Renderer* renderer);
    ~texture_manager();

    SDL_Texture* load_texture(const string& name, const string& filename);
    SDL_Texture* get_texture(const string& name) const;
    void load_textures_from_folder(const string& folder_path);
    void set_renderer(SDL_Renderer* rend) { renderer = rend; }
    void unload_texture(const string& name);
    void clear();
};

#include <vector>
#include <string>

using namespace std;

class tilemap {
    vector<vector<int>> map_data;
    int tile_width, tile_height;
    int tileset_columns;
    texture_manager* tex_mgr;
    string tileset_name;
public:
    tilemap(texture_manager* mgr, const string& tileset, int tileW, int tileH, int columns);
    void set_map(const vector<vector<int>>& map);
    void render(SDL_Renderer* renderer, int offset_x = 0, int offset_y = 0);
};



#endif