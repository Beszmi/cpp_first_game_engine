#include "game.h"
#include "texture_manager.h"

SDL_Rect scrR, destR;

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
		tex_mgr.load_texture("mcblock", "assets/Grass_Block.png");
		SDL_Texture* mc_tex = tex_mgr.get_texture("mcblock");
		if (mc_tex) {
			SDL_QueryTexture(mc_tex, NULL, NULL, &destR.w, &destR.h);
		}

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
	destR.x = cnt;
}

void Game::render() {
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, tex_mgr.get_texture("mcblock"), NULL, &destR);
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