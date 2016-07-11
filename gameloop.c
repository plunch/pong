#include "gameloop.h"
#include "global.h"

#include "typedefs.h"

#include "simulation.h"
#include "scenerender.h"

#include "input.h"
#include "inputsource_sdl.h"
#include "input/keyboard.h"

#include "audioproxy.h"
#include "menus/winmenu.h"
#include "menus/quitmenu.h"

#include <stdbool.h>

static enum menu_result read_input(struct scene* s, struct input* in, struct inputsourcelist* conf) {
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		inputsource_apply(conf, in, &e);

		switch(e.type) {
			case SDL_QUIT:
				return MNU_QUIT;
			case SDL_KEYUP:
				switch(e.key.keysym.sym) {
					case SDLK_BACKSPACE:
					case SDLK_KP_BACKSPACE:
					case SDLK_ESCAPE:
					case SDLK_CANCEL:
						return MNU_BACK;
				}
				break;
				/*
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym) {
					case SDLK_w:
						s->p1.d = -1;
						break;
					case SDLK_s:
						s->p1.d = 1;
						break;
					case SDLK_UP:
						s->p2.d = -1;
						break;
					case SDLK_DOWN:
						s->p2.d = 1;
						break;
				}
				break;	
			case SDL_KEYUP:
				switch(e.key.keysym.sym) {
					case SDLK_w:
					case SDLK_s:
						s->p1.d = 0;
						break;
					case SDLK_UP:
					case SDLK_DOWN:
						s->p2.d = 0;
						break;
					case SDLK_BACKSPACE:
					case SDLK_KP_BACKSPACE:
					case SDLK_ESCAPE:
					case SDLK_CANCEL:
						return MNU_BACK;
				}
				break;
				*/
		}
	}

	s->p1.d = 0;
	if (in->input[GA_P1_DOWN] > 0) {
		s->p1.d += 1.0;
	}
	if (in->input[GA_P1_UP] > 0) {
		s->p1.d -= 1.0;
	}

	s->p2.d = 0;
	if (in->input[GA_P2_DOWN] > 0) {
		s->p2.d += 1.0;
	}
	if (in->input[GA_P2_UP] > 0) {
		s->p2.d -= 1.0;
	}
	return MNU_NONE;
}

enum menu_result main_loop(struct renderer* r,
                           struct scene* s,
                           struct inputsourcelist* config)
{

	struct menu men;
	struct input in;

	struct render_item* paddle1 = ri_load(r, "paddle1");
	struct render_item* paddle2 = ri_load(r, "paddle2");
	struct render_item* ball = ri_load(r, "ball");
	struct render_item* field = ri_load(r, "field");

	uint32_t next_tick = SDL_GetTicks();
	while(1) {
		unsigned int loops = 0;
		while(SDL_GetTicks() > next_tick && loops < MAX_FRAMESKIP) {
			switch (read_input(s, &in, config)) {
				case MNU_QUIT:
					if (!(create_menu(&men, r) && create_quitmenu(&men))) {
						perror("Create quit menu");
						return MNU_QUIT;
					}
					uint32_t tick_diff = SDL_GetTicks() - next_tick;
					enum menu_result qres = run_menu(&men);
					destroy_quitmenu(&men);
					next_tick = SDL_GetTicks() - tick_diff;
					if (qres == MNU_QUIT)
						return MNU_QUIT;
					break;
				case MNU_BACK:
					play_menu_back();
					return MNU_BACK;
				default:
					break;
			}

			int winner = step_simulation(s);
			if (winner >= 0) {
				if (!(create_menu(&men, r) && create_winmenu(&men, winner))) {
					perror("Create win menu");
					return MNU_QUIT;
				}
				run_menu(&men);
				destroy_winmenu(&men);
				return MNU_BACK;
			}

			next_tick += SKIP_TICKS;
			loops++;
		}

		ri_clear(r);

		draw_scene(s, r, paddle1, paddle2, ball, field);

		ri_flip(r);
	}
}
