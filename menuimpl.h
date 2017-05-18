#ifndef MENUIMPL_H
#define MENUIMPL_H

#include "menu.h"
#include "text.h"

enum action {
	ACT_NONE,
	ACT_BACK,
	ACT_CONFIRM,
	ACT_LEFT,
	ACT_RIGHT,
	ACT_UP,
	ACT_DOWN,
	ACT_POINTERX,
	ACT_POINTERY,
	
	ACT_MAX,
};

struct menu {
	void* userdata;
	struct renderer* renderer;
	struct input_state* input;
	void (*paint)(void*, struct renderer*);
	enum menu_result (*action)(void*, enum action, int x, int y);
};

#endif /* MENUIMPL_H */
