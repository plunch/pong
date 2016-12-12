#ifndef INPUT_IMPL_SDL_H
#define INPUT_IMPL_SDL_H

#include "input/event_buffer.h"

#include <SDL2/SDL.h>

void input_sdl_event(struct input_event_buffer*, SDL_Event*);

struct input_source_id input_sdl_scancode(SDL_Scancode);
struct input_source_id input_sdl_keycode(SDL_Keycode);

int input_sdl_write_id(struct input_source_id, char*, size_t);
struct input_source_id input_sdl_read_id(const char*, size_t);

#endif /* INPUT_IMPL_SDL_H */
