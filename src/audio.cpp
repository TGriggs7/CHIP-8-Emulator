#include "audio.h"
#include <stdio.h>
#include <iostream>

using namespace std;

/*
 * NOTE: This simple SDL audio setup is largely from https://stackoverflow.com/a/45002609
 */

const int AMPLITUDE = 32768;  // seemed to give the best buzzing sound
const int SAMPLE_RATE = 44100;

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes) {
    Sint16 *buffer = (Sint16*)raw_buffer;
    int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
    int &sample_nr(*(int*)user_data);

    for(int i = 0; i < length; i++, sample_nr++)
    {
        double time = (double)sample_nr / (double)SAMPLE_RATE;
        buffer[i] = (Sint16)(AMPLITUDE * sin(2.0f * M_PI * 441.0f * time)); // render 441 HZ sine wave
    }
}

SDL_audio::SDL_audio() {
  int result = SDL_Init(SDL_INIT_AUDIO);
  if (result != 0) {
    cout << "Audio couldn't be initialized\n";
    exit(0);
  }

  int sample_nr = 0;

  SDL_AudioSpec want;
  want.freq = SAMPLE_RATE; // number of samples per second
  want.format = AUDIO_S16SYS; // sample type (here: signed short i.e. 16 bit)
  want.channels = 1; // only one channel
  want.samples = 2048; // buffer-size
  want.callback = audio_callback; // function SDL calls periodically to refill the buffer
  want.userdata = &sample_nr; // counter, keeping track of current sample number

  SDL_AudioSpec have;
  result = SDL_OpenAudio(&want, &have);
  if (result != 0) {
    cout << "Couldn't open audio\n";
    exit(0);
  }
  if (want.format != have.format) {
    cout << "Failed to get audio spec\n";
    exit(0);
  }

  is_paused = true;
}

void SDL_audio::play() {
  SDL_PauseAudio(0);
  is_paused = false;
}

void SDL_audio::pause() {
  SDL_PauseAudio(1);
  is_paused = true;
}

