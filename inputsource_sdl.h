#ifndef INPUTSOURCE_SDL_H
#define INPUTSOURCE_SDL_H

#include <SDL2/SDL.h>

#include "input.h"

struct inputsource_sdl {
	void* data;
	void (*apply)(void*, struct input*, SDL_Event*);
};

struct inputsourcelist {
	struct inputsource_sdl* sources;
	size_t len;
};

void inputsource_apply(struct inputsourcelist*, struct input*, SDL_Event*);

#endif /* INPUTSOURCE_SDL_H */
