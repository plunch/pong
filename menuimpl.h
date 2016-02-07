#ifndef MENUIMPL_H
#define MENUIMPL_H

#include "menu.h"

enum action {
	ACT_NONE,
	ACT_BACK,
	ACT_CONFIRM,
	ACT_LEFT,
	ACT_RIGHT,
	ACT_UP,
	ACT_DOWN,
};

struct menu {
	void* userdata;
	void (*paint)(void*, SDL_Renderer*);
	enum menu_result (*action)(void*, enum action);
};

#endif /* MENUIMPL_H */
