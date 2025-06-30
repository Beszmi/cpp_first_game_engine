#include "texture_manager.h"

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