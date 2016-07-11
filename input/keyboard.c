#include "keyboard.h"

#include "typedefs.h"

static void keychange(struct keyboard_mapping* m,
                      struct input* in,
                      SDL_Keycode code,
                      real new_input)
{
	for(size_t i = 0; i < m->len; ++i) {
		if (m->entries[i].keycode == code) {
			in->input[m->entries[i].action] = new_input;
		}
	}
}

void kbinput_apply(struct keyboard_mapping* m, struct input* in, SDL_Event* e)
{
	switch(e->type) {
		case SDL_KEYDOWN:
			keychange(m, in, e->key.keysym.sym, 1);
			return;
		case SDL_KEYUP:
			keychange(m, in, e->key.keysym.sym, 0);
			return;
		default:
			return;
	}
}

static void sdl_inputmapping_func(void* data, struct input* in, SDL_Event* e)
{
	kbinput_apply(data, in, e);
}

void kbinput_make_inputsource(struct inputsource_sdl* target, struct keyboard_mapping* source)
{
	target->data = source;
	target->apply = &sdl_inputmapping_func;
}