#include <iostream>
#include <unistd.h>
#include "chip8.h"
#include "graphics.h"
#include "audio.h"
#include <SDL2/SDL.h>

using namespace std;

#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

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
  SDL_gfx gfx = SDL_gfx();
  SDL_audio audio = SDL_audio();

  // load the rom into memory
  chip8.load(argv[1]);
  
  while (1) {
    chip8.run_cycle();

    if (chip8.reg_sound > 0 && audio.is_paused) {
      audio.play();
    } 
    else if (chip8.reg_sound == 0 && !audio.is_paused) {
      audio.pause();
    }
   
    while (SDL_PollEvent(&gfx.event)) {
      if (gfx.event.type == SDL_QUIT) {
        exit(0);
      }

      if (gfx.event.type == SDL_KEYDOWN) {
        if (gfx.event.key.keysym.sym == SDLK_ESCAPE) {
          exit(0);
        }

        for (int i = 0; i < 16; i++) {
          if (gfx.event.key.keysym.sym == keypad[i]) {
            chip8.key_down[i] = true;
          }
        }
      }

      if (gfx.event.type == SDL_KEYUP) {
        for (int i = 0; i < 16; i++) {
          if (gfx.event.key.keysym.sym == keypad[i]) {
            chip8.key_down[i] = false;
          }
        }
      }
    }
      
    if (chip8.redraw) {
      for (int i = 0; i < WINDOW_Y_MAX; i++) {
        for (int j = 0; j < WINDOW_X_MAX; j++) {
          gfx.pixels[WINDOW_X_MAX * i + j] = 0xFF000000;
          if (chip8.window[j][i]) {
            gfx.pixels[WINDOW_X_MAX * i + j] |= 0x00FFFFFF;
          }
        }
      }

      SDL_UpdateTexture(gfx.texture, NULL, gfx.pixels, 256);
      SDL_RenderClear(gfx.renderer);
      SDL_RenderCopy(gfx.renderer, gfx.texture, NULL, NULL);
      SDL_RenderPresent(gfx.renderer);

      chip8.redraw = false;
    }
      
    usleep(2400);
  }
}




