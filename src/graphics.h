#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include "chip8.h"

class SDL_gfx {
public:
	uint32_t pixels[WINDOW_X_MAX * WINDOW_Y_MAX];
	SDL_Window* window;
	uint32_t window_width;
	uint32_t window_height;
	SDL_Renderer* renderer;
	SDL_Texture* sdlTexture;
	SDL_Event event;

	SDL_gfx();
	void SDL_ERR(const char* err_msg);
};

#endif