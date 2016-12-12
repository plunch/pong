#ifndef INPUTPERSISTENCE_H
#define INPUTPERSISTENCE_H

#include "input/state.h"

#include <SDL2/SDL_rwops.h>


int input_state_write(struct input_state*, SDL_RWops*);

int input_state_read(struct input_state*, SDL_RWops*);

#endif /* INPUTPERSISTENCE_H */
