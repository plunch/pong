#ifndef GLOOP_H
#define GLOOP_H

#include <SDL2/SDL.h>

#include "simulation.h"
#include "input/kernel.h"
#include "menu.h"
#include "render.h"
#include "gameaction.h"

#define BACK_INTENT GAMEACTION_MAX

// 60 ticks per second
#define SKIP_TICKS (1000 / 60)
#define MAX_FRAMESKIP 1

enum menu_result main_loop(struct renderer*,
                           struct scene*,
                           struct input_kernel*,
                           struct input_context*,
			   struct input_context*);

#endif /* GLOOP_H */
