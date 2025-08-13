#include "game_obj.hpp"

//OBJECT
GameObject::GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr, bool show_it): name(name) {
	show = show_it;
	obj_tex = tex_mgr.get_texture(texture);
	set_loc_position(0, 0);
	transform.setWorld(0, 0);
	int w, h;
	SDL_QueryTexture(obj_tex, nullptr, nullptr, &w, &h);
	src_rect = { 0, 0, w, h };
	dst_rect = { 0, 0, float(w), float(h) };
}

GameObject::GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr, int x, int y, bool show_it) : name(name) {
	show = show_it;
	obj_tex = tex_mgr.get_texture(texture);
	set_loc_position(x, y);
	transform.setWorld(x, y);
	int w, h;
	SDL_QueryTexture(obj_tex, nullptr, nullptr, &w, &h);
	src_rect = { 0, 0, w, h };
	dst_rect = { (float)round(transform.worldX), (float)round(transform.worldY), float(w), float(h) };
}

GameObject::GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr, GameObject_cluster* prn, bool show_it): name(name) {
	show = show_it;
	obj_tex = tex_mgr.get_texture(texture);
	transform.parent = prn->get_transform();
	set_loc_position(0, 0);

	int w, h;
	SDL_QueryTexture(obj_tex, nullptr, nullptr, &w, &h);
	transform.computeWorld();
	src_rect = { 0, 0, w, h };
	dst_rect = { (float)round(transform.worldX), (float)round(transform.worldY), float(w), float(h) };
}

GameObject::GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr, int x, int y, GameObject_cluster* prn, bool show_it): name(name) {
	show = show_it;
	obj_tex = tex_mgr.get_texture(texture);
	transform.parent = prn->get_transform();
	set_loc_position(x, y);

	int w, h;
	SDL_QueryTexture(obj_tex, nullptr, nullptr, &w, &h);
	transform.computeWorld();
	src_rect = { 0, 0, w, h };
	dst_rect = { (float)round(transform.worldX), (float)round(transform.worldY), float(w), float(h)};
}

GameObject::GameObject(const GameObject& rhs) : name(rhs.name), obj_tex(rhs.obj_tex), src_rect(rhs.src_rect), dst_rect(rhs.dst_rect), show(rhs.show) {
	set_loc_position(rhs.transform.localX, rhs.transform.localY);
	transform.setWorld(rhs.transform.worldX, rhs.transform.worldY);
}

void GameObject::set_dst_rect(double x, double y) {
	dst_rect.x = static_cast<int>(std::lround(x));
	dst_rect.y = static_cast<int>(std::lround(y));
}

void GameObject::update(double dt, double speed) {
	set_loc_position(transform.localX + (speed * dt), transform.localY + (speed * dt));
	transform.computeWorld();
	dst_rect.x = static_cast<int>(std::lround(transform.worldX));
	dst_rect.y = static_cast<int>(std::lround(transform.worldY));
}

void GameObject::render(SDL_Renderer* ren, const Camera& cam) const {
	if (show == true) {
		SDL_FRect camDst = dst_rect;
		camDst.x -= cam.x;
		camDst.y -= cam.y;
		SDL_RenderCopyF(ren, obj_tex, &src_rect, &camDst);
	}
}

std::unique_ptr<GameObject> GameObject::clone() const {
	return std::make_unique<GameObject>(*this);
}

SDL_Texture* GameObject::get_tex() {
	return obj_tex;
}

SDL_Texture* GameObject::get_tex() const {
	return obj_tex;
}

//CONTAINER
void Game_obj_container::update_all(double dtSeconds, double speed) {
	for (auto& [_, obj] : objects) {
		obj->update(dtSeconds, speed);
	}
}
void Game_obj_container::render_all(SDL_Renderer* ren, const Camera& cam) const {
	for (const auto& [_, obj] : objects) obj->render(ren, cam);
}

//OBJECT CLUSTER

void GameObject_cluster::add_item_local(const GameObject& obj_in, int lx, int ly, bool show_in_clust) {
	auto p = obj_in.clone();
	p->get_transform()->parent = get_transform();
	p->get_transform()->setLocal((float)lx, (float)ly);
	p->set_show(show_in_clust);
	items.push_back(std::move(p));
	get_transform()->markDirty();
}

void GameObject_cluster::add_item_world(const GameObject& obj_in, bool show_in_clust) {
	auto p = obj_in.clone();
	p->get_transform()->parent = get_transform();
	this->get_transform()->computeWorld();
	float lx = obj_in.get_transform()->worldX - this->get_transform()->worldX;
	float ly = obj_in.get_transform()->worldY - this->get_transform()->worldY;
	p->get_transform()->setLocal(lx, ly);
	p->set_show(show_in_clust);
	items.push_back(std::move(p));
}

void GameObject_cluster::update(double dt, double speed) {
	//speed = rand() % 400;
	get_transform()->computeWorld();
	GameObject::update(dt, speed);

	for (auto& c : items) {
		c->update(dt, speed);
	}
}

void GameObject_cluster::render(SDL_Renderer* ren, const Camera& cam) const {
	//render self
	GameObject::render(ren, cam);
	if (SDL_GetError()[0] != '\0') {
		SDL_Log("[SDL] RenderCopy (cluster) error: %s", SDL_GetError());
		SDL_ClearError();
	}

	//render owned
	for (auto& c : items) {
		c->render(ren, cam);
	}
}