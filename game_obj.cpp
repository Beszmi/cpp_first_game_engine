#include "game_obj.hpp"

GameObject::GameObject(const string texture, SDL_Renderer* ren, texture_manager* tex_mgr) {
	renderer = ren;
	obj_tex = tex_mgr->get_texture(texture);
}

void GameObject::update() {

}

void GameObject::render() {

}

GameObject::~GameObject() {

}