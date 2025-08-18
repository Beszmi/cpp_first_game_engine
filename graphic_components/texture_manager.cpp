#include "texture_manager.hpp"

namespace fs = std::filesystem;

texture_manager::texture_manager(SDL_Renderer* renderer) : renderer(renderer) {}

texture_manager::~texture_manager() { clear(); }

namespace {
    inline std::string to_lower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return s;
    }

    inline std::string last_segment(const std::string& key) {
        const auto p = key.find_last_of('/');
        return (p == std::string::npos) ? key : key.substr(p + 1);
    }
}

auto texture_manager::find_iter_by_name(const std::string& name) -> decltype(textures)::iterator{
    auto it = textures.find(name);
    if (it != textures.end()) return it;

    if (name.find('/') == std::string::npos) {
        decltype(textures)::iterator found = textures.end();
        size_t hits = 0;
        for (auto iter = textures.begin(); iter != textures.end(); ++iter) {
            if (last_segment(iter->first) == name) {
                found = iter;
                ++hits;
                if (hits > 1) break;
            }
        }
        if (hits == 1) return found;
        if (hits > 1) {
            std::cerr << "Ambiguous texture name '" << name
                      << "' matches multiple keys. Use full path like 'folder/" << name << "'.\n";
        }
    }
    return textures.end();
}

//const version
auto texture_manager::find_iter_by_name(const std::string& name) const -> decltype(textures)::const_iterator{
    auto it = textures.find(name);
    if (it != textures.end()) return it;

    if (name.find('/') == std::string::npos) {
        decltype(textures)::const_iterator found = textures.end();
        size_t hits = 0;
        for (auto iter = textures.cbegin(); iter != textures.cend(); ++iter) {
            if (last_segment(iter->first) == name) {
                found = iter;
                ++hits;
                if (hits > 1) break;
            }
        }
        return (hits == 1) ? found : textures.end();
    }
    return textures.end();
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
    return (it != textures.end()) ? it->second : nullptr;
}

void texture_manager::load_textures_from_folder(const std::string& folder_path) {
    const fs::path base = folder_path;

    std::error_code ec;
    for (fs::recursive_directory_iterator it(base, ec), end; it != end; it.increment(ec)) {
        if (ec) {
            std::cerr << "Filesystem iteration error: " << ec.message() << "\n";
            break;
        }

        const fs::directory_entry& entry = *it;
        if (!entry.is_regular_file()) continue;

        const fs::path& p = entry.path();
        std::string ext = to_lower(p.extension().string());
        if (ext != ".png" && ext != ".jpg" && ext != ".jpeg") continue;

        // key = relative_dir / stem  (no extension), using '/' separators
        std::error_code ec2;
        fs::path rel_dir = fs::relative(p.parent_path(), base, ec2);
        if (ec2) rel_dir.clear();

        fs::path key_path = (rel_dir.empty() || rel_dir == ".") ? p.stem() : (rel_dir / p.stem());
        std::string key = key_path.generic_string(); // '/' on all platforms

        load_texture(key, p.string());
    }
}

void texture_manager::unload_texture(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        SDL_DestroyTexture(it->second);
        textures.erase(it);
    }
}

bool texture_manager::has(const string& name) {
    auto it = textures.find(name);
    return (it != textures.end()) ? true : false;
}

void texture_manager::clear() {
    for (auto& [name, texture] : textures) {
        SDL_DestroyTexture(texture);
    }
    textures.clear();
}