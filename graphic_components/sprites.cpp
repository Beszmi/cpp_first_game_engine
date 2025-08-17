#include "sprites.hpp"

sprite_component::sprite_component() : obj_tex(nullptr) {}

sprite_component::sprite_component(const std::string& texture, const texture_manager& tex_mgr) {
	obj_tex = tex_mgr.get_texture(texture);
	if (!obj_tex) { std::cerr << "sprite Texture not found for '" << texture << "'\n"; return; }
}

void sprite_component::set_tex(const std::string& texture, const texture_manager& tex_mgr) {
	obj_tex = tex_mgr.get_texture(texture);
	if (!obj_tex) { std::cerr << "streched background Texture not found for '" << texture << "'\n"; return; }

}

void sprite_component::render(SDL_Renderer* ren, const SDL_Rect* src_rect, const SDL_Rect* dst_rect, const Camera& cam) const { // no camera movement
	SDL_RenderCopy(ren, obj_tex, src_rect, dst_rect);
}

void sprite_component::render(SDL_Renderer* ren, const SDL_Rect* src_rect, const SDL_FRect* dst_rect, const Camera& cam, double scale) const {
	SDL_FRect camDst = *dst_rect;
	camDst.x -= cam.x;
	camDst.y -= cam.y;
	camDst.w *= scale;
	camDst.h *= scale;
	SDL_RenderCopyF(ren, obj_tex, src_rect, &camDst);
}

strech_bg::strech_bg() {
	src_rect = { 0, 0, 0, 0 };
	screen = { 0, 0, 0, 0 };
}

strech_bg::strech_bg(const std::string& texture, const texture_manager& tex_mgr, int screen_w, int screen_h) {
	set_tex(texture, tex_mgr);
	int w, h;
	SDL_QueryTexture(get_tex(), nullptr, nullptr, &w, &h);
	src_rect = { 0, 0, w, h };
	screen = { 0, 0, screen_w, screen_h };
}

void strech_bg::set_screen(int screen_w, int screen_h) {
	int w, h;
	SDL_QueryTexture(get_tex(), nullptr, nullptr, &w, &h);
	src_rect = { 0, 0, w, h };
	screen = { 0, 0, screen_w, screen_h };
}

void strech_bg::render(SDL_Renderer* ren, const Camera& cam) const {
	sprite_component::render(ren, &src_rect, &screen, cam);
}