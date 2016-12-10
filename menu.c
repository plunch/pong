#include "menuimpl.h"
#include <assert.h>


enum menu_result run_menu(struct menu* m)
{
	assert(m != NULL && m->paint != NULL && m->action != NULL
	    && m->input != NULL && m->input->values != NULL);

	enum action a = ACT_NONE;
	for(int i = 0; i < MINT_USER_START; ++i) {
		if (m->input->values[i].key & KEY_RISING_EDGE)
			a = i;
	}

	enum menu_result res = m->action(m->userdata, a);

	ri_clear(m->renderer);

	m->paint(m->userdata, m->renderer);

	ri_flip(m->renderer);
	return res;
}

int create_menu(struct menu* m, struct renderer* r, struct input_state* i)
{
	m->renderer = r;
	m->input = i;
	if (!input_state_resize(i, MINT_USER_START))
		return 0;
	return 1;
}

