#include "menuimpl.h"
#include <assert.h>


enum menu_result run_menu(struct menu* m)
{
	assert(m != NULL && m->paint != NULL && m->action != NULL
	    && m->input != NULL && m->input->values != NULL);

	int mx = 0;
	int my = 0;

	enum action a = ACT_NONE;
	for(int i = 0; i < ACT_MAX; ++i) {
		switch(i) {
		case ACT_POINTERX:
			mx = m->input->values[i].relative;
			break;
		case ACT_POINTERY:
			my = m->input->values[i].relative;
			break;
		default:
			if (m->input->values[i].key & KEY_RISING_EDGE)
				a = i;
			break;
		}
	}

	enum menu_result res = m->action(m->userdata, a, mx, my);

	ri_clear(m->renderer);

	m->paint(m->userdata, m->renderer);

	ri_flip(m->renderer);
	return res;
}

int create_menu(struct menu* m, struct renderer* r, struct input_state* i)
{
	m->renderer = r;
	m->input = i;
	if (!input_state_resize(i, ACT_MAX))
		return 0;
	return 1;
}

