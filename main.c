#include "global.h"

#include "gameloop.h"

#include "simulation.h" /* PADDLETHICKNESS */
#include "audio.h"
#include "audioproxy.h"

#include <SDL2/SDL.h>

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
	SDL_Renderer* r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
	if (r == NULL) {
		die_sdl("Create rendering context");
	}

	struct audio_data* aud = audio_init();
	if (aud == NULL)
		die("Initialize audio");

	proxy_init(aud);

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
	s.p1.d = 0;


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

	main_loop(w, r, &s);

	audio_destroy(aud);

	SDL_DestroyRenderer(r);
	SDL_DestroyWindow(w);
	SDL_Quit();
}