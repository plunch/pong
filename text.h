#ifndef TEXT_H
#define TEXT_H

#include <SDL2/SDL.h>

struct textinfo {
	SDL_Texture* b;
	SDL_Rect chars[95]; // ASCII only
};

size_t render_text(SDL_Renderer*, struct textinfo*, SDL_Rect, const char*);

#endif /* TEXT_H */
