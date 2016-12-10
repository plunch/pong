#include "menuimpl.h"
#include <assert.h>

static enum action keyevent(SDL_KeyboardEvent* e)
{
	// Only handle keyup for now
	if (e->type != SDL_KEYUP) return ACT_NONE;

	switch(e->keysym.sym) {
		case SDLK_w:
		case SDLK_UP:
			return ACT_UP;
		case SDLK_s:
		case SDLK_DOWN:
			return ACT_DOWN;
		case SDLK_a:
		case SDLK_LEFT:
			return ACT_LEFT;
		case SDLK_d:
		case SDLK_RIGHT:
			return ACT_RIGHT;
		case SDLK_RETURN:
		case SDLK_RETURN2:
		case SDLK_KP_ENTER:
			return ACT_CONFIRM;
		case SDLK_BACKSPACE:
		case SDLK_KP_BACKSPACE:
		case SDLK_ESCAPE:
		case SDLK_CANCEL:
			return ACT_BACK;
		default: 
			return ACT_NONE;
	}
}

enum menu_result run_menu(struct menu* m)
{
	assert(m != NULL && m->paint != NULL && m->action != NULL);

	//SDL_Event e;
	enum action a = ACT_NONE;
	enum menu_result res = MNU_NONE;
	while(res == MNU_NONE) {
		a = ACT_NONE;
		for(int i = 0; i < MINT_USER_START; ++i) {
			if (m->input->values[i].key & KEY_RISING_EDGE)
				a = i;
		}
		/*
		while(SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					return MNU_QUIT;
				case SDL_KEYUP:
				case SDL_KEYDOWN:
					a = keyevent(&e.key);
					break;
			}
		}
		*/

		res = m->action(m->userdata, a);

		ri_clear(m->renderer);

		m->paint(m->userdata, m->renderer);

		ri_flip(m->renderer);
	}
	return res;
}

int create_menu(struct menu* m, struct renderer* r, struct input_state* i)
{
	m->renderer = r;
	m->input = i;
	if (!input_state_resize(i, MINT_USER_START-1))
		return 0;
	return 1;
}

