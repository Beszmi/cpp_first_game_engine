#include "game_obj.hpp"

//BASE OBJECT
GameObject::GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr, float scale, bool show_it): name(name), scale(scale) {
	show = show_it;
	if (&texture.at(0) == "-") {
		obj_tex = nullptr;
	}
	else {
		obj_tex = tex_mgr.get_texture(texture);
		if (!obj_tex) { std::cerr << "Texture not found for '" << texture << "'\n"; return; }
	}
	set_loc_position(0, 0);
	transform.setWorld(0, 0);
	int w, h;
	SDL_QueryTexture(obj_tex, nullptr, nullptr, &w, &h);
	src_rect = { 0, 0, w, h };
	dst_rect = { 0, 0, float(w), float(h) };
}

GameObject::GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr, int x, int y, float scale, bool show_it) : name(name), scale(scale) {
	show = show_it;
	if (&texture.at(0) == "-") {
		obj_tex = nullptr;
	}
	else {
		obj_tex = tex_mgr.get_texture(texture);
		if (!obj_tex) { std::cerr << "Texture not found for '" << texture << "'\n"; return; }
	}
	set_loc_position(x, y);
	transform.setWorld(x, y);
	int w, h;
	SDL_QueryTexture(obj_tex, nullptr, nullptr, &w, &h);
	src_rect = { 0, 0, w, h };
	dst_rect = { (float)round(transform.worldX), (float)round(transform.worldY), float(w), float(h) };
}

GameObject::GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr, GameObject_cluster* prn, float scale, bool show_it): name(name), scale(scale) {
	show = show_it;
	if (&texture.at(0) == "-") {
		obj_tex = nullptr;
	}
	else {
		obj_tex = tex_mgr.get_texture(texture);
		if (!obj_tex) { std::cerr << "Texture not found for '" << texture << "'\n"; return; }
	}
	transform.parent = prn->get_transform();
	set_loc_position(0, 0);

	int w, h;
	SDL_QueryTexture(obj_tex, nullptr, nullptr, &w, &h);
	transform.computeWorld();
	src_rect = { 0, 0, w, h };
	dst_rect = { (float)round(transform.worldX), (float)round(transform.worldY), float(w), float(h) };
}

GameObject::GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr, int x, int y, GameObject_cluster* prn, float scale, bool show_it): name(name), scale(scale) {
	show = show_it;
	if (&texture.at(0) == "-") {
		obj_tex = nullptr;
	}
	else {
		obj_tex = tex_mgr.get_texture(texture);
		if (!obj_tex) { std::cerr << "Texture not found for '" << texture << "'\n"; return; }
	}
	transform.parent = prn->get_transform();
	set_loc_position(x, y);

	int w, h;
	SDL_QueryTexture(obj_tex, nullptr, nullptr, &w, &h);
	transform.computeWorld();
	src_rect = { 0, 0, w, h };
	dst_rect = { (float)round(transform.worldX), (float)round(transform.worldY), float(w), float(h)};
}

GameObject::GameObject(const GameObject& rhs) : name(rhs.name), obj_tex(rhs.obj_tex), src_rect(rhs.src_rect), dst_rect(rhs.dst_rect), scale(rhs.scale), show(rhs.show) {
	set_loc_position(rhs.transform.localX, rhs.transform.localY);
	transform.setWorld(rhs.transform.worldX, rhs.transform.worldY);
}

void GameObject::set_dst_rect(double x, double y) {
	dst_rect.x = static_cast<float>(std::lround(x));
	dst_rect.y = static_cast<float>(std::lround(y));
}

void GameObject::update(double dt, double speed) {
	set_loc_position(transform.localX + (speed * dt), transform.localY + (speed * dt));
	transform.computeWorld();
	dst_rect.x = static_cast<float>(std::lround(transform.worldX));
	dst_rect.y = static_cast<float>(std::lround(transform.worldY));
}

void GameObject::render(SDL_Renderer* ren, const Camera& cam) const {
	if (show == true) {
		SDL_FRect camDst = dst_rect;
		camDst.x -= cam.x;
		camDst.y -= cam.y;
		camDst.w *= scale;
		camDst.h *= scale;
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
	double lx = obj_in.get_transform()->worldX - this->get_transform()->worldX;
	double ly = obj_in.get_transform()->worldY - this->get_transform()->worldY;
	p->get_transform()->setLocal(lx, ly);
	p->set_show(show_in_clust);
	items.push_back(std::move(p));
}

void GameObject_cluster::update(double dt, double speed) {
	speed = 150;
	get_transform()->computeWorld();
	GameObject::update(dt, speed);

	for (auto& c : items) {
		c->update(dt, 0);
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

void Button::update(double dt, double speed) {
	GameObject::update(0.0, 0.0);
}

//sprite objs

void streched_bg_obj::set_texture(const std::string& texture, const texture_manager& tex_mgr) {
	image.set_tex(texture, tex_mgr);
}

void streched_bg_obj::set_screen(int screen_w, int screen_h) {
	image.set_screen(screen_w, screen_h);
}

void streched_bg_obj::init(const std::string& texture, const texture_manager& tex_mgr, int screen_w, int screen_h) {
	image.set_tex(texture, tex_mgr);
	image.set_screen(screen_w, screen_h);
}

void streched_bg_obj::update(double dt, double speed) {
	GameObject::update(0.0, 0.0);
}

void streched_bg_obj::render(SDL_Renderer* ren, const Camera& cam) const {
	if (does_show()) {
		image.render(ren, cam);
	}
}

//sprite ------------------------------------------------------------

void sprite::add_element(const std::string& texture, const texture_manager& tex_mgr) {
	auto p = std::make_unique<sprite_component>(texture, tex_mgr);
	elements.push_back(std::move(p));
}

void sprite::add_elements_batch(const std::string& name) {

}

std::unique_ptr<sprite_component>& sprite::get_element(size_t idx) {
	return elements.at(idx);
}

size_t sprite::get_current_idx() const {
	return current_element;
	}

void sprite::set_current_idx(size_t idx) {
	if (idx > 1000000000) {
		current_element = elements.size()-1;
	} else if (idx >= elements.size()) {
		current_element = 0;
	} else {
		current_element = idx;
		}
}
 

void sprite::update(double dt, double speed) {
	t = t + dt;
	if (t >= 0.25) {
		switch (state)
		{
		case 0:
			GameObject::update(0.0, 0.0);
			break;
		case 1:
			set_current_idx(++current_element);
			GameObject::update(0.0, 0.0);
			break;
		case -1:
			set_current_idx(current_element - 1);
			GameObject::update(0.0, 0.0);
			break;
		case 2:
			GameObject::update(0.0, 0.0);
			break;
		case -2:
			GameObject::update(0.0, 0.0);
			break;

		default:
			GameObject::update(0.0, 0.0);
			break;
		}
		t = 0;
	}
	
}

void sprite::render(SDL_Renderer* ren, const Camera& cam) const {
	if (does_show()) {
		if (current_element >= elements.size()) {
			std::cout << "sprite currently rendering id too large" << std::endl;
		}
		else if (current_element < 0) {
			std::cout << "sprite currently rendering id too small" << std::endl;
		}
		else {
			elements.at(current_element)->render(ren, &get_src_rect(), &get_dst_rect(), cam, 1.0f);
		}
	}
}