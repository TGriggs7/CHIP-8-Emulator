#include "graphics.h"


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

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
          SDL_TEXTUREACCESS_STREAMING, WINDOW_X_MAX, WINDOW_Y_MAX);

}

void SDL_gfx::redraw(Chip8* chip8) {
	for (int i = 0; i < WINDOW_Y_MAX; i++) {
        for (int j = 0; j < WINDOW_X_MAX; j++) {
          pixels[WINDOW_X_MAX * i + j] = 0xFF000000;
          if (chip8->window[j][i]) {
            pixels[WINDOW_X_MAX * i + j] |= 0x00FFFFFF;
          }
        }
      }

      SDL_UpdateTexture(texture, NULL, pixels, 256);
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
}