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

class GameObject {
	std::string name;
	Transform transform;
	SDL_Texture* obj_tex;
	SDL_Rect src_rect, dst_rect;
public:
	GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr);
	GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr, int x, int y);
	GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr, GameObject_cluster* prn);
	GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr, int x, int y, GameObject_cluster* prn);
	GameObject(const GameObject& rhs);

	const std::string& get_name() const { return name; }
	double get_world_x() const { return transform.worldX; }
	double get_world_y() const { return transform.worldY; }
	Transform* get_transform() { return &transform; }

	void set_loc_position(int x, int y) { transform.setLocal(x, y); }
	void set_loc_x(int x) { transform.localX = x;}
	void set_loc_y(int y) { transform.localY = y;}
	void set_world_pos_force(int x, int y) {transform.worldX= x; transform.worldY = y; }

	virtual void update(double dt);
	virtual void render(SDL_Renderer* ren, const Camera& cam) const;
	virtual ~GameObject() = default;
	virtual void action() {};
	virtual std::unique_ptr<GameObject> clone() const;
	SDL_Texture* get_tex();
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
	void update_all(double dtSeconds);
	void render_all(SDL_Renderer* ren, const Camera& cam) const;
};

class GameObject_cluster: public GameObject {
	std::vector<std::unique_ptr<GameObject>> items;
public:
	using GameObject::GameObject;

	void action()  override { std::cout << "test"; }

	void add_item(const GameObject& obj_in);
	void add_item_zerod(const GameObject& obj_in);

	void update(double dt) override;
	virtual void render(SDL_Renderer* ren, const Camera& cam) const override;
	~GameObject_cluster() = default;
};

#endif