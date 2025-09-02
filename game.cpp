#include "game.hpp"

Game::Game() : tex_mgr(nullptr), screen_w(0), screen_h(0), renderer(nullptr), window(nullptr), run(false) {}

Game::~Game() { clean(); }

namespace {
	bool GetWindowBorders(int& top, int& bottom) {
		top = bottom = 0;

		SDL_Window* test = SDL_CreateWindow("test", 64, 64, 0);
		if (!test) return false;

		SDL_ShowWindow(test);

		const Uint64 start = SDL_GetTicksNS();
		bool status = false;
		while ((SDL_GetTicksNS() - start) < 50'000'000ULL) {
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

	if (target_w <= 1280 && target_h <= 720) {
		screen_scale_factor = 0.75;
	}
	else if (target_w <= 1920 && target_h <= 1080) {
		screen_scale_factor = 1;
	}
	else if (target_w <= 2560 && target_h <= 1440) {
		screen_scale_factor = 1.5;
	}
	else if (target_w <= 3840 && target_h <= 2160) {
		screen_scale_factor = 2;
	}

	if (!fullscreen && target_x >= 0 && target_y >= 0) {
		SDL_SetWindowPosition(window, target_x, target_y);
		run = false;
	}

	if (!SDL_SetRenderVSync(renderer, SDL_RENDERER_VSYNC_ADAPTIVE)) {
		//if adaptive not available unlimited fps
		SDL_SetRenderVSync(renderer, 0);
	}
	SDL_SetWindowMinimumSize(window, 640, 360);
	SDL_SetWindowMaximumSize(window, 3840, 2160);
	std::cout << "SDL init + window/renderer created!\n";
	SDL_GetWindowSizeInPixels(window, &screen_w, &screen_h);

	if (!TTF_Init()) {
		std::cerr << "TTF_Init failed: " << SDL_GetError() << "\n";
		run = false;
		return;
	}

	tex_mgr.set_renderer(renderer);
	tex_mgr.load_textures_from_folder("assets");
	tex_mgr.load_textures_from_folder("assets/sprites");

	if (!tex_mgr.get_texture("dirt")) {
		std::cerr << "[Error] 'dirt' texture not found after loading assets folder.\n";
	}
	SDL_SetTextureScaleMode(tex_mgr.get_texture("dirt"), SDL_SCALEMODE_NEAREST);

	obj_container.spawn_as<GameObject>("mcblock", "mcblock", tex_mgr, 1.0f, true);
	obj_container.spawn_as<GameObject_cluster>("pngtree", "pngtree", tex_mgr, 1.0f, true);
	obj_container.spawn_as<GameObject>("dirt", "dirt", tex_mgr, 1.0f, true);
	obj_container.spawn_as<GameObject>("diamond", "diamond", tex_mgr, 1.0f, true);
	obj_container.spawn_as<Button>("diamond-sword", "diamond-sword", tex_mgr, 0.5, true);
	obj_container.spawn_as<streched_bg_obj>("-", "-", tex_mgr, 1.0f, false, -1);
	obj_container.spawn_as<GameObject>("sussy", "sussy", tex_mgr, 1.0f, true);
	obj_container.spawn_as<sprite>("s1", "-", tex_mgr, 1.0f, true);

	tex_mgr.create_text_texture("test1", "fonts/ARIAL.TTF", 72, "EPIC TESTING TEXT YEAH", Colors::white, Colors::black);

	obj_container.spawn_as<Text_Button>("test1", "test1", tex_mgr, 500, 500, 1.0f, true, 2);


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

	SDL_Texture* dirt = tex_mgr.get_texture("dirt");
	if (!dirt) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Missing texture: dirt");
	}
	else {
		float tw = 0.f, th = 0.f;
		SDL_GetTextureSize(dirt, &tw, &th);
		auto bg = std::make_unique<tilemap>(&tex_mgr,"dirt",(int)tw, (int)th,1,(int)tw, (int)th);

		bg->fill_background(0, 0, 0);
		maps.push_back(std::move(bg));
	}

	auto treeMap = std::make_unique<tilemap>(&tex_mgr,"pngtree",300, 300, 1, 300, 300);
	treeMap->fill_grid(2, 2, 0);
	maps.push_back(std::move(treeMap));

	run = true;
}

void Game::set_cursors(SDL_Cursor* default_cursor_in, SDL_Cursor* pointer_cursor_in) {
	default_cursor = default_cursor_in;
	pointer_cursor = pointer_cursor_in;
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
		case SDL_EVENT_WINDOW_RESIZED:
			SDL_GetWindowSizeInPixels(window, &screen_w, &screen_h);
			if (screen_w <= 1280 && screen_h <= 720) {
				screen_scale_factor = 0.75;
			}
			else if (screen_w <= 1920 && screen_h <= 1080) {
				screen_scale_factor = 1;
			}
			else if (screen_w <= 2560 && screen_h <= 1440) {
				screen_scale_factor = 1.5;
			}
			else if (screen_w <= 3840 && screen_h <= 2160) {
				screen_scale_factor = 2;
			}
			obj_container.set_scale_all(screen_scale_factor);
			break;
		case SDL_EVENT_MOUSE_MOTION: {
			SDL_FPoint W = WindowToWorld(renderer, e.motion.x, e.motion.y, cam);
			//mouse.dx = e.motion.xrel, mouse.dy = e.motion.yrel; //currently unused commented out for performance
			GameObject* hit = obj_container.pick_topmost(W.x, W.y);

			if (hit != hovered) {
				if (hovered) hovered->on_hover_exit(default_cursor);
				hovered = hit;
				if (hovered) hovered->on_hover_enter(pointer_cursor);
			}
			else if (hovered) {
				hovered->on_hover();
			}
		}	break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN: {}
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP: {
			if (e.button.button == SDL_BUTTON_LEFT) {
				SDL_FPoint W = WindowToWorld(renderer, e.button.x, e.button.y, cam);
				if (auto* hit = obj_container.pick_topmost(W.x, W.y)) {
					hit->action();
				}
			}
			//mouse.clicks[b] = e.button.clicks;
		}	break;
		case SDL_EVENT_MOUSE_WHEEL: {
			const float step = 1.1f; // 1.05–1.2
			const float z0 = cam.zoom;
			const float z1 = std::clamp(z0 * std::pow(step, e.wheel.y), 0.1f, 5.0f);

			float mx_i, my_i; SDL_GetMouseState(&mx_i, &my_i);
			float rx, ry;
			WindowToRender(renderer, mx_i, my_i, rx, ry);

			// keep cursor anchored: ΔC = r * (1 - z0/z1), with r in render coords
			cam.x += rx * (1.0f - z0 / z1);
			cam.y += ry * (1.0f - z0 / z1);
			cam.zoom = z1;
		}	break;
		default:
			break;
		}
	}

	int numkeys = 0;
	const bool* keys = SDL_GetKeyboardState(&numkeys);

	const int pan = 600 / 60; // original step; make dt-based if required
	cam.x += (keys[SDL_SCANCODE_D] ? pan : 0) - (keys[SDL_SCANCODE_A] ? pan : 0);
	cam.y += (keys[SDL_SCANCODE_S] ? pan : 0) - (keys[SDL_SCANCODE_W] ? pan : 0);
}


void Game::update(double dtSeconds) {
	//cnt++;
	obj_container.update_all(dtSeconds, 400);
}

void Game::render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	if (SDL_GetError()[0] != '\0') {
		std::cerr << "[SDL] RenderCopy error renderer begin: " << SDL_GetError() << "\n";
		SDL_ClearError();
	}
	SDL_SetRenderScale(renderer, cam.zoom, cam.zoom);
	for (auto& m : maps) {
		m->render(renderer, cam);
	}

	obj_container.render_all(renderer, cam);

	SDL_RenderPresent(renderer);
}

void Game::clean() {
	tex_mgr.clear();
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	std::cout << "clean run" << std::endl;
}

bool Game::running() const {
	return run;
}