#ifndef MENUIMPL_H
#define MENUIMPL_H

#include "menu.h"
#include "text.h"

#define MINT_UP 0
#define MINT_DOWN 1
#define MINT_LEFT 2
#define MINT_RIGHT 3
#define MINT_CONFIRM 4
#define MINT_BACK 5
#define MINT_USER_START 6

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
	struct input_state* input;
	void (*paint)(void*, struct renderer*);
	enum menu_result (*action)(void*, enum action);
};

#endif /* MENUIMPL_H */
