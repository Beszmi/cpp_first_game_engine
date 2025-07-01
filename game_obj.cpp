#include "game_obj.hpp"

GameObject::GameObject(const string texture, SDL_Renderer* ren, texture_manager* tex_mgr) {
	renderer = ren;
	obj_tex = tex_mgr->get_texture(texture);
	xpos = 0;
	ypos = 0;
	SDL_QueryTexture(obj_tex, NULL, NULL, &src_rect.w, &dst_rect.h);
	src_rect.x = 0;
	src_rect.y = 0;
}

void GameObject::update() {
	
}

void GameObject::render() {
	SDL_RenderCopy(renderer, obj_tex, &src_rect, &dst_rect);
}

GameObject::~GameObject() {

}