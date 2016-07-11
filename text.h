#ifndef TEXT_H
#define TEXT_H

#include <SDL2/SDL.h>

struct textinfo {
	SDL_Texture* b;
	SDL_Rect chars[95]; // ASCII only
};

enum text_alignment {
	TA_RIGHT,
	TA_LEFT,
};

size_t render_text(SDL_Renderer*, struct textinfo*, SDL_Rect, enum text_alignment, const char*);

#endif /* TEXT_H */
