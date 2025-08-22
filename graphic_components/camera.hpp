#pragma once
#ifndef camera_hpp
#define camera_hpp
#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

struct Camera {
	float x = 0.f, y = 0.f;
	float zoom = 1.f;
};

inline void ApplyCamera(SDL_Renderer* r, const Camera& cam) {
	SDL_SetRenderScale(r, cam.zoom, cam.zoom);
}

inline void WindowToRender(SDL_Renderer* r, float winX, float winY, float& rx, float& ry) {
	SDL_RenderCoordinatesFromWindow(r, winX, winY, &rx, &ry);
}

inline SDL_FRect WorldToRender(const SDL_FRect& w, const Camera& c) {
	return SDL_FRect{ w.x - c.x, w.y - c.y, w.w, w.h };
}

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