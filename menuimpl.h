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
};

struct menu {
	void* userdata;
	struct renderer* renderer;
	void (*paint)(void*, struct renderer*);
	enum menu_result (*action)(void*, enum action);
};

#endif /* MENUIMPL_H */
