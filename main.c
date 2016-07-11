#include "global.h"

#include "gameloop.h"

#include "render.h"
#include "renderimpl.h"
#include "sdl_renderer.h"

#include "simulation.h" /* PADDLETHICKNESS */
#include "audio.h"
#include "audioproxy.h"

#include "inputsource_sdl.h"
#include "input/keyboard.h"

#include "menus/mainmenu.h"
#include "menus/options.h"

#include "content/asciifont.h"

#include <SDL2/SDL.h>

static enum menu_result game(struct renderer* re, struct inputsourcelist* sources)
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

	return main_loop(re, &s, sources);
}

int main(int argc, char* argv[])
{
	UNUSED(argc);

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
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


	struct menu mainmen;
	if (!(create_menu(&mainmen, &grend) && create_mainmenu(&mainmen))) {
		die("Create main menu");
	}

	struct keyboard_mapping_entry keymap[] = {
		{SDLK_w, GA_P1_UP},
		{SDLK_s, GA_P1_DOWN},
		{SDLK_UP, GA_P2_UP},
		{SDLK_DOWN, GA_P2_DOWN},
	};

	struct keyboard_mapping kconf = { keymap, ARRAYLEN(keymap) };

	struct inputsource_sdl kinput;
	kbinput_make_inputsource(&kinput, &kconf);

	// TODO: Allocate space on heap for input sources
	// to be able too dynamically add and remove inputs
	struct inputsourcelist inplist = { &kinput, 1 };


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
				if (!(create_menu(&opt, &grend) && create_optionsmenu(&opt))) {
					// Abort
					error("Allocate options menu");
					break;
				}

				enum menu_result optres = run_menu(&opt);
				destroy_optionsmenu(&opt);
				if (optres == MNU_QUIT)
					break;
			}
		default:
			goto mainmenu;
	}


	destroy_mainmenu(&mainmen);

#if 0
	audio_destroy(aud);
#endif

	SDL_DestroyRenderer(r);
	SDL_DestroyWindow(w);
	SDL_Quit();
}
