#include <iostream>
#include <unistd.h>
#include "chip8.h"
#include "graphics.h"
#include <SDL2/SDL.h>

using namespace std;

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
      

     // TODO: write redraw funcs 
    if (chip8.redraw) {
      // sdl.redraw(&chip8.window);
      chip8.redraw = false;
    }
      
    usleep(2400);
  }
}




