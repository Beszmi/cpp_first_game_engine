#include "game.hpp"

Game::Game() : tex_mgr(nullptr), screen_w(0), screen_h(0) {
}

Game::~Game() {

}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
	int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "SDL init success!" << std::endl;
		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		screen_w = width;
		screen_h = height;

		const int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
		if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
			std::cerr << "IMG_Init failed: " << IMG_GetError() << "\n";
		}

		if (window) {
			std::cout << "Window created!" << std::endl;
		}

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
		if (renderer) {
			std::cout << "renderer created!" << std::endl;
		}
		else {
			std::cerr << "Failed to create renderer: "
				<< SDL_GetError() << "\n";
			run = false;
			return;
		}

		tex_mgr.set_renderer(renderer);
		tex_mgr.load_textures_from_folder("assets");

		if (!tex_mgr.get_texture("dirt")) {
			std::cerr << "[Error] 'dirt' texture not found after loading assets folder.\n";
		}

		obj_container.spawn("mcblock", "mcblock", renderer, tex_mgr);
		obj_container.spawn("pngtree", renderer, tex_mgr);
		obj_container.spawn("dirt", renderer, tex_mgr);

		SDL_Texture* dirtTex = tex_mgr.get_texture("dirt");
		int tex_w, tex_h;
		SDL_QueryTexture(dirtTex, nullptr, nullptr, &tex_w, &tex_h);
		auto bg = std::make_unique<tilemap>(&tex_mgr, "dirt", tex_w, tex_h, 1);
		bg->fill_background(screen_w, screen_h, 0);
		maps.push_back(std::move(bg));

		run = true;
	}
	else {
		run = false;
	}
}

void Game::handleEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT: run = false; break;
			default: break;
			}
	}
}

void Game::update() {
	//cnt++;
	obj_container.update_all();
}

void Game::render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	if (SDL_GetError()[0] != '\0') {
		std::cerr << "[SDL] RenderCopy error rendereer eleje: " << SDL_GetError() << "\n";
		SDL_ClearError();
	}
	for (auto& layer : maps) {
		layer->render(renderer);
	}
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
	if (auto obj = obj_container.get("dirt")) {
		obj->render();
	}
	else {
		std::cerr << "[ERROR] GameObject 'dirt' not found!\n";
	}

	SDL_RenderPresent(renderer);
}

void Game::clean() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	IMG_Quit();
	SDL_Quit();
	std::cout << "clean run" << std::endl;
}

bool Game::running() {
	return run;
}