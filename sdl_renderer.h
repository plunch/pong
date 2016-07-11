#ifndef SDLRENDERER_H
#define SDLRENDERER_H

#include "render.h"
#include <SDL2/SDL.h>
#include "text.h"

struct sdl_renderer {
	SDL_Window* wind;
	SDL_Renderer* rend;
	struct textinfo* text;
};

struct render_item {
	Uint8 r, g, b;
	int fill;
};

int create_sdl_renderer(struct renderer* rend, SDL_Window* w, SDL_Renderer* r, struct textinfo* ti);

#endif /* SDLRENDERER_H */
