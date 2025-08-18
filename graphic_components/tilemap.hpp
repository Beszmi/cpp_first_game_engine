#pragma once
#ifndef tilemap_h
#define tilemap_h
#include "texture_manager.hpp"

class tilemap {
    int tile_width, tile_height;
    int draw_tile_width, draw_tile_height;
    int tileset_columns;
    texture_manager* tex_mgr;
    string tileset_name;

    std::vector<std::vector<int>> map_data;

    SDL_Texture* cache_texture;
    int cache_width, cache_height;
    bool cache_dirty;
public:
    tilemap(texture_manager* mgr,const std::string& tileset, int tileW, int tileH, int columns, int drawW = 0, int drawH = 0);
    void set_map(const vector<vector<int>>& map);
    void fill_grid(int rows, int cols, int tile_index);
    void fill_background(int screen_w, int screen_h, int tile_index);
    void render(SDL_Renderer* renderer, int offset_x = 0, int offset_y = 0);
    ~tilemap();
private:
    void rebuild_cache(SDL_Renderer* renderer);
};

#endif