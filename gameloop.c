#include "gameloop.h"
#include "global.h"

#include "typedefs.h"

#include "simulation.h"
#include "scenerender.h"


/*#include "input.h"*/
/*#include "inputsource_sdl.h"*/
/*#include "input/keyboard.h"*/

#include "gameaction.h"
#include "input/impl/sdl.h"

#include "audioproxy.h"
#include "menus/winmenu.h"
#include "menus/quitmenu.h"

#include <stdbool.h>
#include <assert.h>

static enum menu_result read_input(struct scene* s,
                                   struct input_kernel* kern,
				   struct input_state* input,
                                   struct input_event_buffer* events)
{
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		input_sdl_event(events, &e);

		switch(e.type) {
			case SDL_QUIT:
				return MNU_QUIT;
		}
	}

	input_kernel_consume(kern, events, SDL_GetTicks());

	if (input->values[GA_P1_MOVE_DOWN].key & KEY_STATE_DOWN)
		s->p1.d = 1;
	else if (input->values[GA_P1_MOVE_UP].key & KEY_STATE_DOWN)
		s->p1.d = -1;
	else
		s->p1.d = 0;

	if (input->values[GA_P2_MOVE_DOWN].key & KEY_STATE_DOWN)
		s->p2.d = 1;
	else if (input->values[GA_P2_MOVE_UP].key & KEY_STATE_DOWN)
		s->p2.d = -1;
	else
		s->p2.d = 0;

	return MNU_NONE;
}

enum menu_result main_loop(struct renderer* r,
                           struct scene* s,
                           struct input_kernel* input)
{
	enum menu_result retval = MNU_NONE;

	struct menu men;

	struct render_item* paddle1 = ri_load(r, "paddle1");
	struct render_item* paddle2 = ri_load(r, "paddle2");
	struct render_item* ball = ri_load(r, "ball");
	struct render_item* field = ri_load(r, "field");

	struct input_context menu_context = {
		{0, NULL, 0, NULL},
		0,
		"Menu",
	};

	struct input_context game_context = {
		{0, NULL, 0, NULL},
		0,
		"Game",
	};

	input_state_create(&game_context.state, GAMEACTION_MAX);

	pllist_append(&input->contexts, &game_context);
	pllist_append(&input->contexts, &menu_context);

	struct input_event_buffer input_buffer;
	if (!input_event_buffer_init(&input_buffer)) {
		perror("Allocate event buffer");
		return MNU_QUIT;
	}

	uint32_t next_tick = SDL_GetTicks();
	while(1) {
		unsigned int loops = 0;
		while(SDL_GetTicks() > next_tick && loops < MAX_FRAMESKIP) {
			switch (read_input(s, input,
			                   &game_context.state, &input_buffer)) {
				case MNU_QUIT:
					if (!(create_menu(&men, r, &menu_context.state)
					   && create_quitmenu(&men))) {
						perror("Create quit menu");
						return MNU_QUIT;
					}
					uint32_t tick_diff = SDL_GetTicks() - next_tick;
					enum menu_result qres = run_menu(&men);
					destroy_quitmenu(&men);
					next_tick = SDL_GetTicks() - tick_diff;
					if (qres == MNU_QUIT) {
						retval = MNU_QUIT;
						goto exit;
					}
					break;
				case MNU_BACK:
					play_menu_back();
					retval = MNU_BACK;
					goto exit;
				default:
					break;
			}

			int winner = step_simulation(s, 1);
			if (winner >= 0) {
				if (!(create_menu(&men, r, &menu_context.state) 
				   && create_winmenu(&men, winner))) {
					perror("Create win menu");
					retval = MNU_QUIT;
					goto exit;
				}
				run_menu(&men);
				destroy_winmenu(&men);
				retval = MNU_BACK;
				goto exit;
			}

			next_tick += SKIP_TICKS;
			loops++;
		}

		ri_clear(r);

		draw_scene(s, r, paddle1, paddle2, ball, field);

		ri_flip(r);
	}

exit:
	input_event_buffer_free(&input_buffer);
	ri_destroy(r, paddle1);
	ri_destroy(r, paddle2);
	ri_destroy(r, ball);
	ri_destroy(r, field);
	assert(retval != MNU_NONE);
	return retval;
}
