#ifndef GLOOP_H
#define GLOOP_H

#include <SDL2/SDL.h>

#include "simulation.h"
#include "input/kernel.h"
#include "menu.h"
#include "render.h"

// 60 ticks per second
#define SKIP_TICKS (1000 / 60)
#define MAX_FRAMESKIP 1

enum menu_result main_loop(struct renderer*,
                           struct scene*,
                           struct input_kernel*);

#endif /* GLOOP_H */
