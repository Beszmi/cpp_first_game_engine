#pragma once
#ifndef camera_hpp
#define camera_hpp
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

struct Camera {
    float x = 0;
	float y = 0;
    float zoom = 1.f;
};

static inline SDL_FRect worldToScreen(const SDL_FRect& w, const Camera& c) {
    SDL_FRect s;
    s.w = w.w * c.zoom;
    s.h = w.h * c.zoom;
    s.x = (w.x - c.x) * c.zoom;
    s.y = (w.y - c.y) * c.zoom;
    return s;
}

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

#endif