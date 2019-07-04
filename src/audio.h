#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

class SDL_audio {
public:
	SDL_audio();
	bool is_paused;
	void play();
	void pause();
};

#endif