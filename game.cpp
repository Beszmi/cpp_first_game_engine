#include "game.hpp"

Game::Game() : tex_mgr(nullptr) {

}

Game::~Game() {

}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
	int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "SDL init success!" << std::endl;
		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (window) {
			std::cout << "Window created!" << std::endl;
		}

		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer) {
			SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
			std::cout << "renderer created!" << std::endl;
		}

		tex_mgr.set_renderer(renderer);
		tex_mgr.load_textures_from_folder("assets");
		obj_container.spawn("mcblock", "mcblock", renderer, tex_mgr);
		obj_container.spawn("pngtree", renderer, tex_mgr);
		obj_container.spawn("dirt", renderer, tex_mgr);

		run = true;
	}
	else {
		run = false;
	}
}

void Game::handleEvents() {
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type)
	{
	case SDL_QUIT:
		run = false;
		break;

	default:
		break;
	}
}

void Game::update() {
	cnt++;
	obj_container.update_all();
}

void Game::render() {
	SDL_RenderClear(renderer);
	if (auto obj = obj_container.get("mcblock")) {
		obj->render();
	}
	else {
		std::cerr << "[ERROR] GameObject 'mcblock' not found!\n";
	}
	if (auto obj = obj_container.get("pngtree")) {
		obj->render();
	}
	else {
		std::cerr << "[ERROR] GameObject 'pngtree' not found!\n";
	}
	SDL_RenderPresent(renderer);
}

void Game::clean() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	TTF_Quit();
	SDL_Quit();
	std::cout << "clean run" << std::endl;
}

bool Game::running() {
	return run;
}