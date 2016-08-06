#ifndef INPUT_KEYBOARD_H
#define INPUT_KEYBOARD_H

#include "../input.h"
#include "../inputsource_sdl.h"

#include <SDL2/SDL.h>

struct keyboard_mapping_entry {
	SDL_Keycode pcode;
	SDL_Keycode ncode;
	enum game_action action;
};

struct keyboard_mapping {
	struct keyboard_mapping_entry* entries;
	size_t len;
};

/*
void kbinput_unmap_key(struct keyboard_mapping*, SDL_Keycode);
void kbinput_map(struct keyboard_mapping*, SDL_Keycode, enum game_action);

#define kbinput_set(km, code, action) \
do { \
	kbinput_unmap_key(km, code); \
	kbinput_map(km, code, action); \
} while (0)
*/

void kbinput_make_inputsource(struct inputsource_sdl*, struct keyboard_mapping*);

#endif /* INPUT_KEYBOARD_H */
