#include "render.h"

static int rintit(real v)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbad-function-cast"
	return (int)rint(v);
#pragma GCC diagnostic pop
}

void draw_scene(struct scene* s, SDL_Renderer* re)
{
	SDL_SetRenderDrawColor(re, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(re);

	int w, h;
	SDL_GetRendererOutputSize(re, &w, &h);

	// Center
	SDL_SetRenderDrawColor(re, 190, 190, 190, SDL_ALPHA_OPAQUE);

	SDL_RenderDrawLine(re, rintit(s->w / 2), 0, rintit(s->w / 2), rintit(s->h));


	SDL_Rect r;
	r.x = 0; r.y = 0;
	r.w = rintit(s->w); r.h = rintit(s->h);
	SDL_RenderDrawRect(re, &r);



	SDL_SetRenderDrawColor(re, 255, 255, 255, SDL_ALPHA_OPAQUE);


	// Ball

	r.x = rintit(s->b.x);
	r.y = rintit(s->b.y);
	r.w = BALLSIZE;
	r.h = BALLSIZE;

	SDL_RenderFillRect(re, &r);

	
	// Player 1

	r.x = rintit(s->p1.x);
	r.y = rintit(s->p1.y);
	r.w = PADDLETHICKNESS;
	r.h = rintit(s->p1.width);

	SDL_RenderFillRect(re, &r);


	// Player 1
	//
	r.x = rintit(s->p2.x);
	r.y = rintit(s->p2.y);
	r.h = rintit(s->p2.width);

	SDL_RenderFillRect(re, &r);

	r.x = 30;
	r.y = 30;
	r.w = 20;
	r.h = 70;
	for (unsigned int i = 0; i < s->p1pt; ++i) {
		SDL_RenderFillRect(re, &r);

		r.x += 50;
	}

	r.x = rintit(s->w) - 30 - 20;
	for(unsigned int i = 0; i < s->p2pt; ++i) {
		SDL_RenderFillRect(re, &r);

		r.x -= 50;
	}
}
