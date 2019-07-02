#include <iostream>
#include <unistd.h>
#include "chip8.h"
#include "graphics.h"
#include <SDL2/SDL.h>

using namespace std;
extern uint8_t* keypad;


int main(int argc, char** argv) {
  if (argc != 2) {
    cout << "RUH ROH RAGGY! No file attached\n";
    exit(0);
  }

  Chip8 chip8 = Chip8();
  SDL_gfx sdl = SDL_gfx();

  // load the rom into memory
  chip8.load(argv[1]);
  
  while (1) {
    chip8.run_cycle();
   

    while (SDL_PollEvent(&sdl.event)) {
      if (sdl.event.type == SDL_QUIT) {
        exit(0);
      }

      if (sdl.event.type == SDL_KEYDOWN) {
        if (sdl.event.key.keysym.sym == SDLK_ESCAPE) {
          exit(0);
        }

        for (int i = 0; i < 16; i++) {
          if (sdl.event.key.keysym.sym == keypad[i]) {
            chip8.key_down[i] = true;
          }
        }
      }

      if (sdl.event.type == SDL_KEYUP) {
        for (int i = 0; i < 16; i++) {
          if (sdl.event.key.keysym.sym == keypad[i]) {
            chip8.key_down[i] = false;
          }
        }
      }
    }
      
    if (chip8.redraw) {
      for (int i = 0; i < WINDOW_Y_MAX; i++) {
        for (int j = 0; j < WINDOW_X_MAX; j++) {
          sdl.pixels[WINDOW_X_MAX * i + j] = (0x00FFFFFF * chip8.window[j][i]) | 0xFF000000;
        }
      }

      SDL_UpdateTexture(sdl.sdlTexture, NULL, sdl.pixels, 256);
      SDL_RenderClear(sdl.renderer);
      SDL_RenderCopy(sdl.renderer, sdl.sdlTexture, NULL, NULL);
      SDL_RenderPresent(sdl.renderer);

      chip8.redraw = false;
    }
      
    usleep(2400);
  }
}




