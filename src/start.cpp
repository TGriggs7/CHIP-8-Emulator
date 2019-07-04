#include <iostream>
#include <unistd.h>
#include "chip8.h"
#include "graphics.h"
#include <SDL2/SDL.h>

using namespace std;

#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

const int AMPLITUDE = 32768;
const int SAMPLE_RATE = 44100;

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes)
{
    Sint16 *buffer = (Sint16*)raw_buffer;
    int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
    int &sample_nr(*(int*)user_data);

    for(int i = 0; i < length; i++, sample_nr++)
    {
        double time = (double)sample_nr / (double)SAMPLE_RATE;
        buffer[i] = (Sint16)(AMPLITUDE * sin(2.0f * M_PI * 441.0f * time)); // render 441 HZ sine wave
    }
}

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

  if(SDL_Init(SDL_INIT_AUDIO) != 0) SDL_Log("Failed to initialize SDL: %s", SDL_GetError());

  int sample_nr = 0;

  SDL_AudioSpec want;
  want.freq = SAMPLE_RATE; // number of samples per second
  want.format = AUDIO_S16SYS; // sample type (here: signed short i.e. 16 bit)
  want.channels = 1; // only one channel
  want.samples = 2048; // buffer-size
  want.callback = audio_callback; // function SDL calls periodically to refill the buffer
  want.userdata = &sample_nr; // counter, keeping track of current sample number

  SDL_AudioSpec have;
  if(SDL_OpenAudio(&want, &have) != 0) SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to open audio: %s", SDL_GetError());
  if(want.format != have.format) SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Failed to get the desired AudioSpec");

  // SDL_PauseAudio(0); // start playing sound
  // SDL_Delay(1000); // wait while sound is playing
  // SDL_PauseAudio(1); // stop playing sound

  bool is_paused = true;
  
  while (1) {
    chip8.run_cycle();

    cout << int(chip8.reg_sound) << "\n";

    if (chip8.reg_sound > 0 && is_paused) {
      cout << "PLAY\n";
      SDL_PauseAudio(0);
      is_paused = false;
    } 
    else if (chip8.reg_sound == 0 && !is_paused) {
      cout << "PAUSE\n";
      SDL_PauseAudio(1);
      is_paused = true;
    }
   
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
          sdl.pixels[WINDOW_X_MAX * i + j] = 0xFF000000;
          if (chip8.window[j][i]) {
            sdl.pixels[WINDOW_X_MAX * i + j] |= 0x00FFFFFF;
          }
        }
      }

      SDL_UpdateTexture(sdl.texture, NULL, sdl.pixels, 256);
      SDL_RenderClear(sdl.renderer);
      SDL_RenderCopy(sdl.renderer, sdl.texture, NULL, NULL);
      SDL_RenderPresent(sdl.renderer);

      chip8.redraw = false;
    }
      
    usleep(2400);
  }
}




