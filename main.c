#include "global.h"

#include "gameloop.h"

#include "render.h"
#include "renderimpl.h"
#include "sdl_renderer.h"

#include "simulation.h" /* PADDLETHICKNESS */
#include "audio.h"
#include "audioproxy.h"

#include "input/impl/sdl.h"
#include "input_persistence.h"

#include "menus/mainmenu.h"
#include "menus/options.h"

#include "content/asciifont.h"

#include <SDL2/SDL.h>


enum high_state {
	ST_INGAME,
	ST_MAINMENU,
	ST_OPTIONSMENU,
	ST_QUITTING,
};

static enum menu_result game(struct renderer* re,
                             struct input_kernel* k,
                             struct input_context* m)
{
	struct scene s;
	
	s.w = 1200;
	s.h = 800;
	s.p1pt = 0;
	s.p2pt = 0;

	s.bs = 2;
	s.b.x = 600;
	s.b.y = 400;
	s.b.dx = -2;
	s.b.dy = 0;

	s.p1.x = 50;
	s.p1.y = 300 - 100;
	s.p1.width = 200;
	s.p1.s = 6;
	s.p1.d = 0;

	s.p2.x = 1200 - 50 - PADDLETHICKNESS;
	s.p2.y = 300;
	s.p2.width = 200;
	s.p2.s = 6;
	s.p2.d = 0;

	return main_loop(re, &s, k, m);
}

int main(int argc, char* argv[])
{
	UNUSED(argc);

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK) != 0) {
		die_sdl("Initialize SDL");
	}

	SDL_Window* w = SDL_CreateWindow(argv[0],
	                                 SDL_WINDOWPOS_UNDEFINED,
	                                 SDL_WINDOWPOS_UNDEFINED,
	                                 1200, 800,
	                                 SDL_WINDOW_HIDDEN|
	                                 SDL_WINDOW_RESIZABLE/*|
	                                 SDL_WINDOW_FULLSCREEN_DESKTOP*/);
	if (w == NULL) {
		die_sdl("Create window");
	}
	SDL_Renderer* r = SDL_CreateRenderer(w, -1, 0);
	if (r == NULL) {
		die_sdl("Create rendering context");
	}

#if 0
	struct audio_data* aud = audio_init();
	if (aud == NULL)
		die("Initialize audio");

	proxy_init(aud);
#endif

	struct textinfo asciifont;
	if (!load_content_ascii(&asciifont, r)){
		die("Load font texture");
	}

	struct renderer grend = RENDER_EMPTY;
	create_sdl_renderer(&grend, w, r, &asciifont);

	struct input_kernel input = { NULL };
	struct input_context menu_ctx = {
		{0, NULL, 0, NULL},
		0,
		"Menu"
	};
	pllist_append(&input.contexts, &menu_ctx);

	SDL_RWops* menumap = SDL_RWFromFile("content/menumap.txt", "r");
	if (menumap != NULL) {
		if (!input_state_read(&menu_ctx.state, menumap))
			fprintf(stderr, "Cannot read menumap.txt: %s",
			        SDL_GetError());
		SDL_RWclose(menumap);
	} else {
		SDL_ClearError();

		input_state_add_mapping(&menu_ctx.state,
		                        input_sdl_keycode(SDLK_UP),
					ACT_UP);
		input_state_add_mapping(&menu_ctx.state,
		                        input_sdl_scancode(SDL_SCANCODE_W),
					ACT_UP);
		input_state_add_mapping(&menu_ctx.state,
		                        input_sdl_keycode(SDLK_DOWN),
					ACT_DOWN);
		input_state_add_mapping(&menu_ctx.state,
		                        input_sdl_scancode(SDL_SCANCODE_S),
					ACT_DOWN);

		input_state_add_mapping(&menu_ctx.state,
		                        input_sdl_keycode(SDLK_LEFT),
					ACT_LEFT);
		input_state_add_mapping(&menu_ctx.state,
		                        input_sdl_scancode(SDL_SCANCODE_A),
					ACT_LEFT);
		input_state_add_mapping(&menu_ctx.state,
		                        input_sdl_keycode(SDLK_RIGHT),
					ACT_RIGHT);
		input_state_add_mapping(&menu_ctx.state,
		                        input_sdl_scancode(SDL_SCANCODE_D),
					ACT_RIGHT);

		input_state_add_mapping(&menu_ctx.state,
		                        input_sdl_keycode(SDLK_RETURN),
					ACT_CONFIRM);
		input_state_add_mapping(&menu_ctx.state,
		                        input_sdl_keycode(SDLK_RETURN2),
					ACT_CONFIRM);
		input_state_add_mapping(&menu_ctx.state,
		                        input_sdl_keycode(SDLK_KP_ENTER),
					ACT_CONFIRM);

		input_state_add_mapping(&menu_ctx.state,
		                        input_sdl_keycode(SDLK_BACKSPACE),
					ACT_BACK);
		input_state_add_mapping(&menu_ctx.state,
		                        input_sdl_keycode(SDLK_KP_BACKSPACE),
					ACT_BACK);
		input_state_add_mapping(&menu_ctx.state,
		                        input_sdl_keycode(SDLK_ESCAPE),
					ACT_BACK);
		input_state_add_mapping(&menu_ctx.state,
		                        input_sdl_keycode(SDLK_CANCEL),
					ACT_BACK);

		SDL_RWops* out = SDL_RWFromFile("content/menumap.txt", "w");
		if (out != NULL) {
			if (!input_state_write(&menu_ctx.state, out))
				fprintf(stderr,
				        "Cannot write menumap.txt: %s",
			        	SDL_GetError());
			SDL_RWclose(out);
		} else {
			SDL_ClearError();
		}
	}


#if 0
	struct keyboard_mapping_entry keymap[] = {
		{SDLK_w, GA_P1_MOVE_UP},
		{SDLK_s, GA_P1_MOVE_DOWN},
		{SDLK_UP, GA_P2_MOVE_UP},
		{SDLK_DOWN, GA_P2_MOVE_DOWN}
	};

	SDL_Joystick* joys[2] = { NULL, NULL };

	struct joyaxis_mapping jamap[4];
	size_t nj = 0;
	int njr = SDL_NumJoysticks();
	if (njr < 0)
		die_sdl("Get number of joysticks");
	else
		nj = (size_t)njr;

	if (nj > 0) {
		joys[0] = SDL_JoystickOpen(0);

		if (joys[0] == NULL)
			die_sdl("Open joystick 0");

		jamap[0].joystick = SDL_JoystickInstanceID(joys[0]);
		jamap[0].axis = 1;
		jamap[0].deadzone = 100;
		jamap[0].negative = 1;
		jamap[0].action = GA_P1_MOVE_UP;

		jamap[1].joystick = jamap[0].joystick;
		jamap[1].axis = 1;
		jamap[1].deadzone = 100;
		jamap[1].negative = 0;
		jamap[1].action = GA_P1_MOVE_DOWN;
	}
	if (nj > 1) {
		joys[1] = SDL_JoystickOpen(1);

		if (joys[1] == NULL)
			die_sdl("Open joystick 1");

		jamap[2].joystick = SDL_JoystickInstanceID(joys[1]);
		jamap[2].axis = 1;
		jamap[2].deadzone = 100;
		jamap[2].negative = 1;
		jamap[2].action = GA_P2_MOVE_UP;

		jamap[3].joystick = jamap[0].joystick;
		jamap[3].axis = 1;
		jamap[3].deadzone = 100;
		jamap[3].negative = 0;
		jamap[3].action = GA_P2_MOVE_DOWN;
	}


	struct joystick_mapping jconf = {
		jamap,           NULL, NULL, NULL,
		nj > 2 ? 4 : nj*2, 0,    0,    0,
	};

	struct keyboard_mapping kconf = { keymap, ARRAYLEN(keymap) };


	struct inputsource_sdl input[2];
	kbinput_make_inputsource(&input[0], &kconf);
	jsinput_make_inputsource(&input[1], &jconf);

	// TODO: Allocate space on heap for input sources
	// to be able too dynamically add and remove inputs
	struct inputsourcelist inplist = { input, ARRAYLEN(input) };

#endif


#if DEBUG
	/* Clear screen sky blue.
	 * Sky blue becauese it gives us a better hint
	 * of where we draw and where we arent drawing
	 * (because black is usually drawn..)
	 */
	SDL_SetRenderDrawColor(r, 165, 206, 235, SDL_ALPHA_OPAQUE);
#else
	SDL_SetRenderDrawColor(r, 0, 0, 0, SDL_ALPHA_OPAQUE);
#endif
	SDL_RenderClear(r);
	SDL_RenderPresent(r);

	SDL_ShowWindow(w);

	/*
mainmenu:
	switch(run_menu(&mainmen)) {
		case MNU_FORWARD:
			if (game(&grend, &inplist) == MNU_QUIT)
				break;
			goto mainmenu;
		case MNU_QUIT:
		case MNU_BACK:
			break;
		case MNU_OPT1:
			{
				struct menu opt;
				if (!(create_menu(&opt, &grend, &menu_ctx.state)
				   && create_optionsmenu(&opt))) {
					// Abort
					error("Allocate options menu");
					break;
				}

				enum menu_result optres = run_menu(&opt);
				if (optres == MNU_QUIT)
					break;
			}
		default:
			goto mainmenu;
	}
	*/

	struct menu mainmen;
	if (!(create_menu(&mainmen, &grend, &menu_ctx.state)
	   && create_mainmenu(&mainmen)))
		die("Create main menu");

	struct menu opt;
	if (!(create_menu(&opt, &grend, &menu_ctx.state)
	   && create_optionsmenu(&opt)))
	   	die("Create options menu");

	struct input_event_buffer events;
	if (!input_event_buffer_init(&events))
		die("Create input event buffer");

	input_state_reset(&menu_ctx.state);

	enum high_state st = ST_MAINMENU;
	while(st != ST_QUITTING) {
		SDL_Event e;
		while(SDL_PollEvent(&e)) {
			input_sdl_event(&events, &e);

			switch(e.type) {
				case SDL_QUIT:
					return MNU_QUIT;
			}
		}

		input_state_step(&menu_ctx.state);
		input_kernel_consume(&input, &events, SDL_GetTicks());

		switch(st) {
			case ST_QUITTING: break;
			case ST_MAINMENU:
				menu_ctx.active = 1;
				switch(run_menu(&mainmen)) {
					case MNU_FORWARD:
						st = ST_INGAME;
						continue;
					case MNU_QUIT:
					case MNU_BACK:
						st = ST_QUITTING;
						continue;
					case MNU_OPT1:
						st = ST_OPTIONSMENU;
						continue;
					default:
						break;
				}
				break;
			case ST_INGAME:
				menu_ctx.active = 0;
				switch(game(&grend, &input, &menu_ctx)) {
					case MNU_QUIT:
						st = ST_QUITTING;
						continue;
					case MNU_BACK:
						st = ST_MAINMENU;
						input_state_reset(&menu_ctx.state);
						continue;
					default:
						break;
				}
				break;
			case ST_OPTIONSMENU:
				menu_ctx.active = 1;
				switch(run_menu(&opt)) {
					case MNU_QUIT:
						st = ST_QUITTING;
						continue;
					case MNU_NONE:
						break;
					default:
						st = ST_MAINMENU;
						continue;
				}
		}
	}

	destroy_mainmenu(&mainmen);
	destroy_optionsmenu(&opt);


#if 0
	audio_destroy(aud);
#endif


#if 0
	for(size_t i = 0; i < ARRAYLEN(joys); ++i) {
		if (joys[i] != NULL && SDL_JoystickGetAttached(joys[i]))
			SDL_JoystickClose(joys[i]);
	}
#endif

	pllist_clear(&input.contexts);
	input_state_release(&menu_ctx.state);
	destroy_sdl_renderer(&grend);
	SDL_DestroyRenderer(r);
	SDL_DestroyWindow(w);
	SDL_Quit();
}

