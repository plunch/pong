#ifndef GLOOP_H
#define GLOOP_H

#include <SDL2/SDL.h>

#include "simulation.h"

// 60 ticks per second
#define SKIP_TICKS (1000 / 60)
#define MAX_FRAMESKIP 1

void main_loop(SDL_Window*, SDL_Renderer*, struct scene*);

#endif /* GLOOP_H */
