#include "tilemap.hpp"
using namespace std;

tilemap::tilemap(texture_manager* mgr, const std::string& tileset, int tileW, int tileH, int columns, int drawW, int drawH) :
    tex_mgr(mgr), tileset_name(tileset), tile_width(tileW), tile_height(tileH), tileset_columns(columns), cache_texture(nullptr), cache_width(0), cache_height(0),
    draw_tile_width(drawW > 0 ? drawW : tileW), draw_tile_height(drawH > 0 ? drawH : tileH), cache_dirty(true) {}

void tilemap::set_map(const vector<vector<int>>& map) {
    map_data = map;
    cache_dirty = true;
}

void tilemap::fill_grid(int rows, int cols, int tile_index) {
    map_data.assign(rows, std::vector<int>(cols, tile_index));
    cache_dirty = true;
}

void tilemap::fill_background(int screen_w, int screen_h, int tile_index) {
    int cols = (screen_w + tile_width - 1) / tile_width;
    int rows = (screen_h + tile_height - 1) / tile_height;
    map_data.assign(rows, std::vector<int>(cols, tile_index));
    cache_dirty = true;
}

tilemap::~tilemap() {
    if (cache_texture) {
        SDL_DestroyTexture(cache_texture);
    }
}

void tilemap::rebuild_cache(SDL_Renderer* renderer) {
    SDL_Texture* tileset = tex_mgr->get_texture(tileset_name);
    if (!tileset) {
        std::cerr << "[tilemap] Missing tileset: " << tileset_name << "\n";
        return;
    }

    int tex_w, tex_h;
    SDL_QueryTexture(tileset, nullptr, nullptr, &tex_w, &tex_h);
    int cols_in_tileset = tex_w / tile_width;
    int rows_in_tileset = tex_h / tile_height;
    int max_tiles = cols_in_tileset * rows_in_tileset;

    int rows = map_data.size();
    int cols = rows ? map_data[0].size() : 0;
    cache_width = cols * draw_tile_width;
    cache_height = rows * draw_tile_height;

    if (cache_texture) SDL_DestroyTexture(cache_texture);
    cache_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        cache_width, cache_height
    );
    if (!cache_texture) {
        std::cerr << "[tilemap] Failed to create cache: " << SDL_GetError() << "\n";
        return;
    }

    SDL_SetTextureBlendMode(cache_texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, cache_texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int idx = map_data[r][c];
            if (idx < 0 || idx >= max_tiles) continue;

            SDL_Rect src{
                (idx % cols_in_tileset) * tile_width,
                (idx / cols_in_tileset) * tile_height,
                tile_width,
                tile_height
            };
            SDL_Rect dst{
                c * draw_tile_width,
                r * draw_tile_height,
                draw_tile_width,
                draw_tile_height
            };
            SDL_RenderCopy(renderer, tileset, &src, &dst);
        }
    }

    SDL_SetRenderTarget(renderer, nullptr);
    cache_dirty = false;
}

void tilemap::render(SDL_Renderer* renderer, int offset_x, int offset_y) {
    if (cache_dirty) {
        rebuild_cache(renderer);
    }
    if (!cache_texture) return;

    SDL_Rect dst{ offset_x, offset_y,
                   cache_width, cache_height };
    SDL_RenderCopy(renderer, cache_texture, nullptr, &dst);
}