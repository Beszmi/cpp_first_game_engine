#pragma once
#ifndef camera_hpp
#define camera_hpp
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include "SDL.h"
#include "SDL_image.h"

struct Camera {
    int x = 0;
    int y = 0;
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

static inline SDL_Rect roundRect(const SDL_FRect& r) {
    SDL_Rect out;
    out.x = (int)std::round(r.x);
    out.y = (int)std::round(r.y);
    out.w = (int)std::round(r.w);
    out.h = (int)std::round(r.h);
    return out;
}

#endif