#include "game_obj.hpp"
//named objects
GameObject::GameObject(const std::string& name_in, const std::string& texture, SDL_Renderer* ren, texture_manager& tex_mgr) {
	renderer = ren;
	name = name_in;
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
	//dst_rect.w = static_cast<int>(dst_rect.w * 1.01);
	//dst_rect.h = static_cast<int>(dst_rect.h * 1.01);
}

void GameObject::render() {
	SDL_RenderCopy(renderer, obj_tex, &src_rect, &dst_rect);
}

void Game_obj_container::spawn(const std::string& name, const std::string& texName, SDL_Renderer* ren, texture_manager& tm) {
	objects.emplace(
		name,
		std::make_unique<GameObject>(name, texName, ren, tm)
	);
}

void Game_obj_container::spawn(const std::string& texName, SDL_Renderer* ren, texture_manager& tm) {
	objects.emplace(
		texName,
		std::make_unique<GameObject>(texName, texName, ren, tm)
	);
}

GameObject* Game_obj_container::get(const std::string& name) {
	auto it = objects.find(name);
	return (it != objects.end()) ? it->second.get() : nullptr;
}

void Game_obj_container::update_all() {
	for (auto& [_, obj] : objects) {
		int direction_x = rand() % 2;
		int direction_y = rand() % 2;
		if (direction_x == 0) {
			obj->set_x(obj->get_x() - 2);
		}
		else {
			obj->set_x(obj->get_x() + 2);
		}
		if (direction_y == 0) {
			obj->set_y(obj->get_y() - 2);
		}
		else {
			obj->set_y(obj->get_y() + 2);
		}
		obj->update();
	}
}
void Game_obj_container::render_all() {
	for (auto& [_, obj] : objects) obj->render();
}