#include "game.hpp"

Game::Game() : tex_mgr(nullptr), screen_w(0), screen_h(0) {}

Game::~Game() {}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
	int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "SDL init success!" << std::endl;

		SDL_Rect usableBounds;
		if (SDL_GetDisplayUsableBounds(0, &usableBounds) != 0) {
			std::cout << "SDL_GetDisplayUsableBounds Error: " << SDL_GetError() << std::endl;
			window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		}
		else {
			SDL_Window* tempWin = SDL_CreateWindow(
				"Temp",
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				100, 100,
				SDL_WINDOW_HIDDEN
			);

			int topBorder = 0, leftBorder = 0, bottomBorder = 0, rightBorder = 0;
			if (SDL_GetWindowBordersSize(tempWin, &topBorder, &leftBorder, &bottomBorder, &rightBorder) != 0) {
				std::cout << "SDL_GetWindowBordersSize Error: " << SDL_GetError() << std::endl;
				screen_w = width;
				screen_h = height;
			}
			SDL_DestroyWindow(tempWin);

			usableBounds.y += topBorder;
			usableBounds.h -= topBorder;

			window = SDL_CreateWindow(title, usableBounds.x, usableBounds.y, usableBounds.w, usableBounds.h, flags);

			screen_w = usableBounds.w;
			screen_h = usableBounds.h;
		}

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
		tex_mgr.load_textures_from_folder("assets/sprites");

		if (!tex_mgr.get_texture("dirt")) {
			std::cerr << "[Error] 'dirt' texture not found after loading assets folder.\n";
		}

		obj_container.spawn_as<GameObject>("mcblock", "mcblock", tex_mgr, 1.0f, true);
		obj_container.spawn_as<GameObject_cluster>("pngtree", "pngtree", tex_mgr, 1.0f, true);
		obj_container.spawn_as<GameObject>("dirt", "dirt", tex_mgr, 1.0f, true);
		obj_container.spawn_as<GameObject>("diamond", "diamond", tex_mgr, 1.0f, true);
		obj_container.spawn_as<Button>("diamond-sword", "diamond-sword", tex_mgr, 0.5, true);
		obj_container.spawn_as<streched_bg_obj>("-", "-", tex_mgr, 1.0f, false);
		obj_container.spawn_as<GameObject>("sussy", "sussy", tex_mgr, 1.0f, true);
		obj_container.spawn_as<sprite>("s1", "s1", tex_mgr, 1.0f, true);

		sprite& cocacola = *obj_container.get<sprite>("s1");

		for (int i = 1;; ++i) {
			std::string key = "sprites/s" + std::to_string(i);
			if (!tex_mgr.has(key)) break;
			cocacola.add_element(key, tex_mgr);
		}

		streched_bg_obj& floppa = *obj_container.get<streched_bg_obj>("-");
		floppa.init("floppa", tex_mgr, screen_w, screen_h);
		
		Button& button_obj = *obj_container.get<Button>("diamond-sword");
		int temp = button_obj.GameObject::get_dst_rect().w;
		button_obj.get_transform()->setWorld(screen_w - temp, 0);
		button_obj.get_transform()->computeWorld();

		GameObject_cluster& test_obj = *obj_container.get<GameObject_cluster>("pngtree");
		test_obj.add_item_local(*obj_container.get("dirt"), 50, -40, true);
		test_obj.add_item_local(*obj_container.get("diamond"), -10, 20, true);

		SDL_Texture* dirtTex = tex_mgr.get_texture("dirt");
		int tex_w, tex_h;
		SDL_QueryTexture(dirtTex, nullptr, nullptr, &tex_w, &tex_h);
		auto bg = std::make_unique<tilemap>(&tex_mgr, "dirt", tex_w, tex_h, 1);
		bg->fill_background(screen_w, screen_h, 0);
		maps.push_back(std::move(bg));

		auto treeMap = std::make_unique<tilemap>(&tex_mgr, "pngtree", 300, 300, 1, 300, 300);
		treeMap->fill_grid(2, 2, 0);
		maps.push_back(std::move(treeMap));

		run = true;
	}
	else {
		run = false;
	}
}

void Game::handleEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) run = false;
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) run = false;
	}
	const Uint8* keys = SDL_GetKeyboardState(nullptr);
	int pan = 600 / 60;
	if (keys[SDL_SCANCODE_A]) cam.x -= pan;
	if (keys[SDL_SCANCODE_D]) cam.x += pan;
	if (keys[SDL_SCANCODE_W]) cam.y -= pan;
	if (keys[SDL_SCANCODE_S]) cam.y += pan;
}

void Game::update(double dtSeconds) {
	//cnt++;
	obj_container.update_all(dtSeconds);
}

void Game::render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	if (SDL_GetError()[0] != '\0') {
		std::cerr << "[SDL] RenderCopy error renderer begin: " << SDL_GetError() << "\n";
		SDL_ClearError();
	}
	for (auto& layer : maps) {
		layer->render(renderer, -cam.x, -cam.y);
	}

	obj_container.render_all(renderer, cam);

	SDL_RenderPresent(renderer);
}

void Game::clean() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	IMG_Quit();
	SDL_Quit();
	std::cout << "clean run" << std::endl;
}

bool Game::running() const {
	return run;
}