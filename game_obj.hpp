#pragma once

#ifndef game_obj_hpp
#define game_obj_hpp
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include "SDL.h"
#include "SDL_image.h"
#include "texture_manager.hpp"
#include "camera.hpp"
#include <cmath>

//TRANSFORM
struct Transform {
	double localX = 0.f, localY = 0.f;
	double worldX = 0.f, worldY = 0.f;
	Transform* parent = nullptr;
	bool dirty = true;

	void setLocal(double x, double y) { localX = x; localY = y; dirty = true; }
	void setWorld(double x, double y) {
		if (parent) { setLocal(x - parent->worldX, y - parent->worldY); }
		else { localX = x; localY = y; dirty = true; }
	}

	void markDirty() { dirty = true; }

	void computeWorld() {
		if (!dirty) return;
		if (parent) {
			parent->computeWorld();
			worldX = parent->worldX + localX;
			worldY = parent->worldY + localY;
		}
		else {
			worldX = localX; worldY = localY;
		}
		dirty = false;
	}
};

//game objects

class GameObject_cluster; // for ctors

class GameObject {
	std::string name;
	Transform transform;
	SDL_Texture* obj_tex;
	SDL_Rect src_rect;
	SDL_FRect dst_rect;
	bool show;
	float scale;
public:
	GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr, float scale = 1.0f, bool show_it = false);
	GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr, int x, int y, float scale = 1.0f, bool show_it = false);
	GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr, GameObject_cluster* prn, float scale = 1.0f, bool show_it = false);
	GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr, int x, int y, GameObject_cluster* prn, float scale = 1.0f, bool show_it = false);
	GameObject(const GameObject& rhs);

	const std::string& get_name() const { return name; }
	double get_world_x() const { return transform.worldX; }
	double get_world_y() const { return transform.worldY; }
	double get_loc_x() const { return transform.localX; }
	double get_loc_y() const { return transform.localY; }
	Transform* get_transform() { return &transform; }
	const Transform* get_transform() const { return &transform; }

	void set_loc_position(double x, double y) { transform.setLocal(x, y); }
	void set_loc_x(double x) { transform.localX = x; transform.dirty = true; }
	void set_loc_y(double y) { transform.localY = y; transform.dirty = true; }

	virtual void update(double dt, double speed = 400);
	virtual void render(SDL_Renderer* ren, const Camera& cam) const;
	virtual ~GameObject() = default;
	virtual void action() {};
	virtual std::unique_ptr<GameObject> clone() const;

	SDL_Texture* get_tex();
	SDL_Texture* get_tex() const;
	SDL_Rect& get_src_rect() { return src_rect; }
	SDL_FRect& get_dst_rect() { return dst_rect; }
	const SDL_Rect& get_src_rect() const { return src_rect; }
	const SDL_FRect& get_dst_rect() const { return dst_rect; }
	void set_dst_rect(double x, double y);

	float get_scale() const { return scale; }
	void set_scale(float scale_in) { scale = scale_in; }

	bool does_show() const { return show; }
	void set_show(bool v) { show = v; }
};

class Game_obj_container {
	std::unordered_map<std::string, std::unique_ptr<GameObject>> objects;
public:
	template<typename T, typename... Args>
	T* spawn_as(const std::string& name, Args&&... args) {
		auto p = std::make_unique<T>(name, std::forward<Args>(args)...);
		T* raw = p.get();
		objects[name] = std::move(p);
		return raw;
	}

	template<class T = GameObject>
	T* get(const std::string& name) {
		static_assert(std::is_base_of_v<GameObject, T>,
			"T must derive from GameObject");
		auto it = objects.find(name);
		if (it == objects.end()) return nullptr;
		return dynamic_cast<T*>(it->second.get());
	}

	template<class T = GameObject>
	const T* get(const std::string& name) const {
		static_assert(std::is_base_of_v<GameObject, T>,
			"T must derive from GameObject");
		auto it = objects.find(name);
		if (it == objects.end()) return nullptr;
		return dynamic_cast<const T*>(it->second.get());
	}
	void update_all(double dtSeconds, double speed = 400);
	void render_all(SDL_Renderer* ren, const Camera& cam) const;
};

class GameObject_cluster: public GameObject {
	std::vector<std::unique_ptr<GameObject>> items;
public:
	using GameObject::GameObject;

	//void action()  override { std::cout << "test"; }

	void add_item_local(const GameObject& obj_in, int lx, int ly, bool show_in_clust = false);
	void add_item_world(const GameObject& obj_in, bool show_in_clust = false);

	void update(double dt, double speed = 400) override;
	void render(SDL_Renderer* ren, const Camera& cam) const override;
	~GameObject_cluster() = default;
};

// DERIVED OBJECTS --------------------------------------------------------------------------------------

class Button : public GameObject {
	bool pressed = false;
public:
	using GameObject::GameObject;
	//void action()  override { std::cout << "test"; }
	std::unique_ptr<GameObject> clone() const override { return std::make_unique<Button>(*this); }

	void update(double dt, double speed = 400) override;
	using GameObject::render;

	~Button() = default;
};

#endif