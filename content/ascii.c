#include "asciifont.h"

#include <assert.h>

static void fill_charboundsarr(SDL_Rect* r)
{
	int i = 0;
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 21; col++) {
			r[i].x = col * 3;
			r[i].y = row * 5;
			r[i].w = 3;
			r[i].h = 5;
			if (++i >= 95) return;
		}
	}
}

int load_content_ascii(struct textinfo* ti, SDL_Renderer* r)
{
	SDL_Surface* surf = SDL_LoadBMP("content/ascii.bmp");
	if (surf == NULL) {
		return 0;
	}
	SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
	SDL_FreeSurface(surf);

	if (tex == NULL)
		return 0;

	ti->b = tex;
	fill_charboundsarr(ti->chars);
	return 1;
}


