#include "game.h"

SDL_Texture* kockatex;
SDL_Rect scrR, destR;

Game::Game() {

}

Game::~Game() {

}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
	int flags = 0;
	if (fullscreen) {
		flags = SDL_WINDOW_FULLSCREEN;
	}

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
		run = true;
	}
	else {
		run = false;
	}
	SDL_Surface* tmpSurface = IMG_Load("assets/Grass_Block.png");
	kockatex = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_free(tmpSurface);
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
	destR.h = 300;
	destR.w = 300;
	destR.x = cnt;
}

void Game::render() {
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, kockatex, NULL, &destR);
	SDL_RenderPresent(renderer);
}

void Game::clean() {
	SDL_DestroyTexture(kockatex);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	TTF_Quit();
	SDL_Quit();
	std::cout << "clean run" << std::endl;
}

bool Game::running() {
	return run;
}