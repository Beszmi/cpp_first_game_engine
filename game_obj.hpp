#pragma once

#ifndef game_obj_hpp
#define game_obj_hpp
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include "texture_manager.hpp"
#include "camera.hpp"

class GameObject {
	std::string name;
	int xpos, ypos;
	SDL_Texture* obj_tex;
	SDL_Rect src_rect, dst_rect;
public:
	GameObject(const std::string& name, const std::string& texture, const texture_manager& tex_mgr);

	const std::string& get_name() const { return name; }
	int get_x() const { return xpos; }
	int get_y() const { return ypos; }
	void set_position(int x, int y) { xpos = x; ypos = y; }
	void set_x(int x) { xpos = x;}
	void set_y(int y) { ypos = y; }

	virtual void update();
	virtual void render(SDL_Renderer* ren, const Camera& cam) const;
	virtual ~GameObject() = default;
	virtual void say() {};
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

	// Const
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
	//GameObject_cluster(const std::string& name, const std::string& texture, const texture_manager& tex_mgr);
	using GameObject::GameObject;
	void say()  override { std::cout << "test"; }

	//void update();
	//void render(SDL_Renderer* ren) const;
};

#endif