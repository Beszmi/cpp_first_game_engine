#include "game_obj.hpp"
//named objects
GameObject::GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr): name(name) {
	obj_tex = tex_mgr.get_texture(texture);
	xpos = 0;
	ypos = 0;
	int w, h;
	SDL_QueryTexture(obj_tex, nullptr, nullptr, &w, &h);
	src_rect = { 0, 0, w, h };
	dst_rect = { xpos, ypos, w, h };
}

void GameObject::update() {
	dst_rect.x = xpos;
	dst_rect.y = ypos;
}

void GameObject::render(SDL_Renderer* ren, const Camera& cam) const {
	SDL_Rect camDst = dst_rect;
	camDst.x -= cam.x;
	camDst.y -= cam.y;
	SDL_RenderCopy(ren, obj_tex, &src_rect, &camDst);
}

void Game_obj_container::update_all(double dtSeconds) {
	for (auto& [_, obj] : objects) {
		obj->set_position(obj->get_x() + (400 * dtSeconds), obj->get_y() + (400 * dtSeconds));
		obj->update();
	}
}
void Game_obj_container::render_all(SDL_Renderer* ren, const Camera& cam) const {
	for (auto& [_, obj] : objects) obj->render(ren, cam);
}