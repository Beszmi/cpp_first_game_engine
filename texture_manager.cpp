#include "texture_manager.hpp"

#include <filesystem>
namespace fs = std::filesystem;

texture_manager::texture_manager(SDL_Renderer* renderer) : renderer(renderer) {}

texture_manager::~texture_manager() {
	clear();
}

SDL_Texture* texture_manager::load_texture(const std::string& name, const std::string& filename) {
    if (textures.find(name) != textures.end()) {
        return textures[name];
    }

    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << filename << "\n";
        std::cerr << "SDL_image error: " << IMG_GetError() << "\n";
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "Failed to create texture from: " << filename << "\n";
        std::cerr << "SDL error: " << SDL_GetError() << "\n";
        return nullptr;
    }

    textures[name] = texture;
    return texture;
}

SDL_Texture* texture_manager::get_texture(const std::string& name) const {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    }
    return nullptr;
}

void texture_manager::load_textures_from_folder(const std::string& folder_path) {
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.path().extension() == ".png" || entry.path().extension() == ".jpg") {
            if (entry.is_regular_file()) {
                std::string full_path = entry.path().string();
                std::string filename = entry.path().stem().string();
                load_texture(filename, full_path);
            }
        }
    }
}

void texture_manager::unload_texture(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        SDL_DestroyTexture(it->second);
        textures.erase(it);
    }
}

void texture_manager::clear() {
    for (auto& [name, texture] : textures) {
        SDL_DestroyTexture(texture);
    }
    textures.clear();
}

//tilemaps code

tilemap::tilemap(texture_manager* mgr, const std::string& tileset, int tileW, int tileH, int columns): 
    tex_mgr(mgr), tileset_name(tileset), tile_width(tileW), tile_height(tileH), tileset_columns(columns), cache_texture(nullptr), cache_width(0), cache_height(0), cache_dirty(true) {}

void tilemap::set_map(const vector<vector<int>>& map) {
    map_data = map;
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
        std::cerr << "[tilemap] Missing tileset: "
            << tileset_name << "\n";
        return;
    }

    int tex_w, tex_h;
    SDL_QueryTexture(tileset, nullptr, nullptr, &tex_w, &tex_h);
    int rows_in_tileset = tex_h / tile_height;
    int cols_in_tileset = tex_w / tile_width;
    int max_tiles = rows_in_tileset * cols_in_tileset;

    int rows = static_cast<int>(map_data.size());
    int cols = (rows > 0 ? static_cast<int>(map_data[0].size()) : 0);
    cache_width = cols * tile_width;
    cache_height = rows * tile_height;

    if (cache_texture) {
        SDL_DestroyTexture(cache_texture);
    }

    cache_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        cache_width, cache_height
    );
    if (!cache_texture) {
        std::cerr << "[tilemap] Failed to create cache: "
            << SDL_GetError() << "\n";
        return;
    }

    SDL_SetTextureBlendMode(cache_texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, cache_texture);
    SDL_RenderClear(renderer);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int idx = map_data[r][c];
            if (idx < 0 || idx >= max_tiles) continue;

            SDL_Rect src{(idx % tileset_columns) * tile_width,(idx / tileset_columns) * tile_height,tile_width,tile_height};
            SDL_Rect dst{c * tile_width,r * tile_height,tile_width,tile_height
            };
            SDL_RenderCopy(renderer, tileset, &src, &dst);
            if (SDL_GetError()[0] != '\0') {
                std::cerr << "[SDL] RenderCopy error cache vege: " << SDL_GetError() << "\n";
                SDL_ClearError();
            }
        }
    }

    SDL_SetRenderTarget(renderer, nullptr);
    cache_dirty = false;
}

void tilemap::render(SDL_Renderer* renderer, int offset_x, int offset_y){
    if (cache_dirty) {
        rebuild_cache(renderer);
    }
    if (!cache_texture) return;

    SDL_Rect dst{ offset_x, offset_y,
                   cache_width, cache_height };
    SDL_RenderCopy(renderer, cache_texture, nullptr, &dst);
    if (SDL_GetError()[0] != '\0') {
        std::cerr << "[SDL] RenderCopy error tilemap render: " << SDL_GetError() << "\n";
        SDL_ClearError();
    }
}