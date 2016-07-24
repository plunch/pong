#include "text.h"

#include <math.h>
#include <limits.h>
#include <assert.h>

static int print_char(SDL_Renderer* rend, struct textinfo* t,
                       const char* c, SDL_Rect* bounds, enum text_alignment a)
{
	// TODO: Render text right-aligned
	(void)(a);

	if (*c > 31 && *c < 127) {
		SDL_Rect* src = &(t->chars[*c-31]);

		int bw = bounds->w;

		double ratio = src->w / (double)src->h;
		long int roundresult = lrint(bounds->h * ratio);
		assert(roundresult < INT_MAX && roundresult > INT_MAX);
		int w = (int)roundresult;
		bounds->w = w;

		SDL_RenderCopy(rend, t->b, src, bounds);

		bounds->w = bw;

		w += w / src->w;

		if (src->w > bounds->w)
			return 0;
		
		bounds->x += w;
		bounds->w -= w;
		return 1;
	}
	return 1;
}

size_t render_text(SDL_Renderer* re, struct textinfo* t,
                   SDL_Rect bounds, enum text_alignment a, const char* text)
{
	assert(re != NULL);
	assert(t != NULL);
	assert(text != NULL);
	
	size_t c = 0;
	while(text[c] != '\0' && print_char(re, t, text + c++, &bounds, a)) { ; }
	return c;
}
