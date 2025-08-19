#include "game.hpp"

Game::Game() : tex_mgr(nullptr), screen_w(0), screen_h(0) {}

Game::~Game() { clean(); }

namespace {
	bool GetWindowBorders(int& top, int& bottom) {
		top = bottom = 0;

		SDL_Window* test = SDL_CreateWindow("test", 64, 64, 0);
		if (!test) return false;

		SDL_ShowWindow(test);

		const Uint64 start = SDL_GetTicksNS();
		bool status = false;
		while ((SDL_GetTicksNS() - start) < 150'000'000ULL) {
			SDL_PumpEvents();
			status = SDL_GetWindowBordersSize(test, &top, nullptr, &bottom, nullptr);
			if (status) break;
			SDL_Delay(1);
		}

		SDL_DestroyWindow(test);
		return status;
	}
}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
	int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
		run = false;
		return;
	}

	int target_w = width;
	int target_h = height;
	int target_x = -1, target_y = -1;
	SDL_SetWindowMinimumSize(window, 640, 360);
	SDL_SetWindowMaximumSize(window, 3840, 2160);
	SDL_WindowFlags wflags = SDL_WINDOW_HIGH_PIXEL_DENSITY;
	if (fullscreen) {
		wflags |= SDL_WINDOW_FULLSCREEN;
	}
	else {
		wflags |= SDL_WINDOW_RESIZABLE;
		int topBorder = 0, bottomBorder = 0;
		GetWindowBorders(topBorder, bottomBorder);

		if (SDL_DisplayID did = SDL_GetPrimaryDisplay()) {
			SDL_Rect usable{};
			if (SDL_GetDisplayUsableBounds(did, &usable)) {
				target_w = usable.w;
				target_h = usable.h - topBorder;
				target_x = usable.x;
				target_y = usable.y + topBorder;
			}
			else {
				std::cerr << "SDL_GetDisplayUsableBounds failed: " << SDL_GetError() << "\n";
			}
		}
		else {
			std::cerr << "SDL_GetPrimaryDisplay failed: " << SDL_GetError() << "\n";
		}
	}
	//---------------------------------------------------
	//SDL_SetHint(SDL_HINT_RENDER_DRIVER, "vulkan");
	//---------------------------------------------------
	if (!SDL_CreateWindowAndRenderer(title, target_w, target_h, wflags, &window, &renderer)) {
		std::cerr << "SDL_CreateWindowAndRenderer failed: " << SDL_GetError() << "\n";
		run = false;
		return;
	}

	if (!fullscreen && target_x >= 0 && target_y >= 0) {
		SDL_SetWindowPosition(window, target_x, target_y);
		run = false;
	}

	if (!fullscreen && target_x >= 0 && target_y >= 0) {
		SDL_SetWindowPosition(window, target_x, target_y);
		run = false;
	}

	if (!SDL_SetRenderVSync(renderer, SDL_RENDERER_VSYNC_ADAPTIVE)) {
		// fall back to normal vsync
		SDL_SetRenderVSync(renderer, 0);
	}

	std::cout << "SDL init + window/renderer created!\n";
	SDL_GetWindowSizeInPixels(window, &screen_w, &screen_h);

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
	obj_container.spawn_as<sprite>("s1", "-", tex_mgr, 1.0f, true);

	sprite& cocacola = *obj_container.get<sprite>("s1");

	for (int i = 1;; ++i) {
		std::string key = "sprites/s" + std::to_string(i);
		if (!tex_mgr.has(key)) break;
		cocacola.add_element(key, tex_mgr);
	}

	streched_bg_obj& floppa = *obj_container.get<streched_bg_obj>("-");
	floppa.init("floppa", tex_mgr, screen_w, screen_h);
	
	Button& button_obj = *obj_container.get<Button>("diamond-sword");
	float temp = button_obj.GameObject::get_dst_rect().w;
	button_obj.get_transform()->setWorld(screen_w - temp, 0);
	button_obj.get_transform()->computeWorld();

	GameObject_cluster& test_obj = *obj_container.get<GameObject_cluster>("pngtree");
	test_obj.add_item_local(*obj_container.get("dirt"), 50, -40, true);
	test_obj.add_item_local(*obj_container.get("diamond"), -10, 20, true);

	SDL_Texture* dirtTex = tex_mgr.get_texture("dirt");
	float tex_w, tex_h;
	SDL_GetTextureSize(dirtTex, &tex_w, &tex_h);
	auto bg = std::make_unique<tilemap>(&tex_mgr, "dirt", tex_w, tex_h, 1);
	bg->fill_background(screen_w, screen_h, 0);
	maps.push_back(std::move(bg));

	auto treeMap = std::make_unique<tilemap>(&tex_mgr, "pngtree", 300, 300, 1, 300, 300);
	treeMap->fill_grid(2, 2, 0);
	maps.push_back(std::move(treeMap));

	run = true;
}

void Game::handleEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_EVENT_QUIT:
			run = false;
			break;
		case SDL_EVENT_KEY_DOWN:
			if (!e.key.repeat && e.key.scancode == SDL_SCANCODE_ESCAPE) {
				run = false;
			}
			break;
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: 
			SDL_GetWindowSizeInPixels(window, &screen_w, &screen_h);
			break;
		default:
			break;
		}
	}

	//const Uint8* keys = SDL_GetKeyboardState(nullptr);
	int numkeys = 0;
	const bool* keys = SDL_GetKeyboardState(&numkeys);

	const int pan = 600 / 60; // your original step; make dt-based if you like
	cam.x += (keys[SDL_SCANCODE_D] ? pan : 0) - (keys[SDL_SCANCODE_A] ? pan : 0);
	cam.y += (keys[SDL_SCANCODE_S] ? pan : 0) - (keys[SDL_SCANCODE_W] ? pan : 0);
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
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	std::cout << "clean run" << std::endl;
}

bool Game::running() const {
	return run;
}