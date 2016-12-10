#ifndef MENU_H
#define MENU_H

#include "render.h"
#include "input/state.h"

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

enum menu_result run_menu(struct menu*);

int create_menu(struct menu*, struct renderer*, struct input_state*);

#endif /* MENU_H */
