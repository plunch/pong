#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>

enum menu_result {
	MNU_NONE,
	MNU_QUIT,
	MNU_BACK,
	MNU_FORWARD,
	MNU_OPT1,
	MNU_OPT2,
	MNU_OPT3,
	MNU_OPT4,
};

struct menu;

enum menu_result run_menu(SDL_Renderer*, struct menu*);

#endif /* MENU_H */
