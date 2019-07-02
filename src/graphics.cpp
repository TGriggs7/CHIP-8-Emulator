#include "graphics.h"

uint8_t keypad[16] = {
		SDLK_b,
		SDLK_4,
		SDLK_5,
		SDLK_6,
		SDLK_r,
		SDLK_t,
		SDLK_y,
		SDLK_f,
		SDLK_g,
		SDLK_h,
		SDLK_v,
		SDLK_n,
		SDLK_7,
		SDLK_u,
		SDLK_j,
		SDLK_m
	};

void SDL_gfx::SDL_ERR(const char* err_msg) {
	printf("SDL Error: %s\n", err_msg);
	exit(1);
}


SDL_gfx::SDL_gfx() {
	int result = SDL_Init(SDL_INIT_EVERYTHING);
	if (result < 0) {
		SDL_ERR("Initialization failed :(");
	}

	window_width = 1024;
	window_height = 512;

	window = SDL_CreateWindow("CHIP8", 0, 0, window_width, window_height, SDL_WINDOW_SHOWN);
	if (!window) {
		SDL_ERR("Couldn't create window :'(");
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_RenderSetLogicalSize(renderer, window_width, window_height);

	sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
          SDL_TEXTUREACCESS_STREAMING, WINDOW_X_MAX, WINDOW_Y_MAX);

}

