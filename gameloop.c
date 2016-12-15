#include "gameloop.h"
#include "global.h"

#include "typedefs.h"

#include "simulation.h"
#include "scenerender.h"


#include "gameaction.h"
#include "input/impl/sdl.h"

#include "audioproxy.h"
#include "menus/winmenu.h"
#include "menus/quitmenu.h"

#include <stdbool.h>
#include <assert.h>

#define BACK_INTENT GAMEACTION_MAX

enum game_state {
	GAME_RUNNING,
	GAME_PAUSED,
	GAME_WON,
};

static enum menu_result read_input_inner(struct input_kernel* kern,
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

	input_state_step(input);
	input_kernel_consume(kern, events, SDL_GetTicks());
	return MNU_NONE;
}

static enum menu_result read_input(struct scene* s,
                                   struct input_kernel* kern,
				   struct input_state* input,
                                   struct input_event_buffer* events)
{
	enum menu_result res = read_input_inner(kern, input, events);
	if (res == MNU_QUIT)
		return res;

	if (input->values[BACK_INTENT].key & KEY_RISING_EDGE)
		return MNU_BACK;

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
                           struct input_kernel* input,
			   struct input_context* menu_context)
{
	enum menu_result retval = MNU_NONE;

	struct menu men;

	struct render_item* paddle1 = ri_load(r, "paddle1");
	struct render_item* paddle2 = ri_load(r, "paddle2");
	struct render_item* ball = ri_load(r, "ball");
	struct render_item* field = ri_load(r, "field");

	struct input_context game_context = {
		{0, NULL, 0, NULL},
		1,
		"Game",
	};

	input_state_create(&game_context.state, GAMEACTION_MAX+1);

	input_state_add_mapping(&game_context.state,
                        	input_sdl_keycode(SDLK_ESCAPE),
				BACK_INTENT);
	input_state_add_mapping(&game_context.state,
                        	input_sdl_scancode(SDL_SCANCODE_W),
				GA_P1_MOVE_UP);
	input_state_add_mapping(&game_context.state,
                        	input_sdl_scancode(SDL_SCANCODE_S),
				GA_P1_MOVE_DOWN);
	input_state_add_mapping(&game_context.state,
                        	input_sdl_keycode(SDLK_UP),
				GA_P2_MOVE_UP);
	input_state_add_mapping(&game_context.state,
                        	input_sdl_keycode(SDLK_DOWN),
				GA_P2_MOVE_DOWN);


	pllist_append(&input->contexts, &game_context);
	pllist_append(&input->contexts, menu_context);

	struct input_event_buffer input_buffer;
	if (!input_event_buffer_init(&input_buffer)) {
		perror("Allocate event buffer");
		return MNU_QUIT;
	}
	uint32_t tick_diff;


	enum game_state state = GAME_RUNNING;
	uint32_t next_tick = SDL_GetTicks();
	while(1) {
		switch (state) {
			case GAME_PAUSED:
				menu_context->active = 1;
				if (read_input_inner(input,
				                     &menu_context->state,
				                     &input_buffer) 
				      == MNU_QUIT) {
					destroy_quitmenu(&men);
					retval = MNU_QUIT;
					goto exit;
				}
				switch(run_menu(&men)) {
					case MNU_NONE:
						continue;
					case MNU_QUIT:
						destroy_quitmenu(&men);
						retval = MNU_QUIT;
						goto exit;
					default:
						destroy_quitmenu(&men);
						next_tick = SDL_GetTicks() - tick_diff;
						state = GAME_RUNNING;
						continue;
				}
				continue;

			case GAME_WON:
				menu_context->active = 1;
				if (read_input_inner(input,
				                     &menu_context->state,
				                     &input_buffer) 
				    == MNU_QUIT) {
					destroy_winmenu(&men);
					retval = MNU_QUIT;
					goto exit;
				}
				if (run_menu(&men) != MNU_NONE) {
					destroy_winmenu(&men);
					retval = MNU_BACK;
					goto exit;
				}
				continue;
			case GAME_RUNNING:
				menu_context->active = 0;
				unsigned int loops = 0;
				while(SDL_GetTicks() > next_tick && loops < MAX_FRAMESKIP) {
					switch (read_input(s, input,
			                   		&game_context.state, &input_buffer)) {
						case MNU_QUIT:
							tick_diff = SDL_GetTicks() - next_tick;

							if (!(create_menu(&men, r, &menu_context->state)
	   						   && create_quitmenu(&men))) {
								perror("Create quit menu");
								return MNU_QUIT;
							}
							input_state_reset(&menu_context->state);
							state = GAME_PAUSED;
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
						state = GAME_WON;
						if (!(create_menu(&men, r, &menu_context->state) 
				   		   && create_winmenu(&men, winner))) {
							perror("Create win menu");
							retval = MNU_QUIT;
							goto exit;
						}
						input_state_reset(&menu_context->state);
					}

					next_tick += SKIP_TICKS;
					loops++;
				}
	
				ri_clear(r);
		
				draw_scene(s, r, paddle1, paddle2, ball, field);
	
				ri_flip(r);
				break;
		}
	}

exit:
	input_event_buffer_free(&input_buffer);
	pllist_remove(&input->contexts, &game_context);
	pllist_remove(&input->contexts, menu_context);
	input_state_release(&game_context.state);
	ri_destroy(r, paddle1);
	ri_destroy(r, paddle2);
	ri_destroy(r, ball);
	ri_destroy(r, field);
	assert(retval != MNU_NONE);
	return retval;
}
