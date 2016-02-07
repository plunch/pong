#include "gameloop.h"
#include "global.h"

#include "typedefs.h"

#include "simulation.h"
#include "render.h"

#include "audio.h"

#include <stdbool.h>

static enum menu_result read_input(struct scene* s) {
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		switch(e.type) {
			case SDL_QUIT:
				return MNU_QUIT;
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
		}
	}
	return MNU_NONE;
}

enum menu_result main_loop(SDL_Window* w, SDL_Renderer* r, struct scene* s)
{
	UNUSED(w);
	
	uint32_t next_tick = SDL_GetTicks();
	while(1) {
		unsigned int loops = 0;
		while(SDL_GetTicks() > next_tick && loops < MAX_FRAMESKIP) {
			switch (read_input(s)) {
				// TODO: Show 'are you sure?'
				case MNU_QUIT:
					return MNU_QUIT;
				case MNU_BACK:
					return MNU_BACK;
				default:
					break;
			}

			step_simulation(s);

			next_tick += SKIP_TICKS;
			loops++;
		}

		SDL_RenderClear(r);

		draw_scene(s, r);

		SDL_RenderPresent(r);
	}

	return MNU_BACK;
}
