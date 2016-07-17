#include "winmenu.h"

#include "../global.h"

struct winmenu_state {
	struct render_item* gfx;
	int winner;
};

static void winmenu_paint(void* userdata, struct renderer* re)
{
	struct winmenu_state* s = userdata;

	int w, h;
	ri_outputbounds(re, &w, &h);

	int x = 0;
	int y = 0;
	w = w / 2;

	switch (s->winner) {
		default:
			break;
		case 1:
			break;
		case 2:
			x = w / 2;
			break;
	}

	ri_draw(re, s->gfx, x, y, w, h);
}

static enum menu_result winmenu_action(void* userdata, enum action action)
{
	UNUSED(userdata);
	
	if (action != ACT_NONE) 
		return MNU_FORWARD;
	else
		return MNU_NONE;
}

void destroy_winmenu(struct menu* m)
{
	if (m->userdata != NULL) {
		struct winmenu_state* s = m->userdata;
		ri_destroy(m->renderer, s->gfx);
		free(m->userdata);
	}
}

int create_winmenu(struct menu* m, int winner)
{
	struct winmenu_state* state = malloc(sizeof(struct winmenu_state));
	if (state == NULL)
		return 0;

	state->winner = winner;
	state->gfx = ri_load(m->renderer, "winstategfx");

	m->userdata = state;
	m->paint = &winmenu_paint;
	m->action = &winmenu_action;
	return 1;
}
