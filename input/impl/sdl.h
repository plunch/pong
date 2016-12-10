#ifndef INPUT_IMPL_SDL_H
#define INPUT_IMPL_SDL_H

#include "input/event_buffer.h"

#include <SDL2/SDL.h>

void input_sdl_event(struct input_event_buffer*, SDL_Event*);

struct input_source_id input_sdl_keyid(SDL_Scancode);

#endif /* INPUT_IMPL_SDL_H */
