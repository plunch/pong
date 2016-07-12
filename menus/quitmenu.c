#include "quitmenu.h"
#include <assert.h>

#include "global.h"
#include "render.h"
#include "menuimpl.h"
#include "audioproxy.h"

#include "common.h"


enum quitmenu_option {
	QUITM_DONTEXIT = 0,
	QUITM_EXIT = 1,
};

struct quitmenu_state {
	enum quitmenu_option selected;
};

static void quitmenu_paint(void* userdata, struct renderer* re)
{
	struct quitmenu_state* m = userdata;

	int rw, rh, w, h;
	ri_outputbounds(re, &rw, &rh);

	if (rw > rh) {
		w = rh / 2;
		h = rh;
	} else {
		h = rw * 2;
		w = rw;
	}

	int x, y;
	if (w < rw) {
		y = 0;
		x = rw / 2 - w / 2;
	} else {
		x = 0;
		y = 0;
	}
	y = (h / 6) - 50 / 2;
	ri_drawtext(re, RTA_LEFT, RTS_DEFAULT, x, y, w, 50, "QUIT NOW?");

	y = h / 3;

	y += draw_option(re, m->selected == QUITM_DONTEXIT, x, y, w, h, "NO");
	y += draw_option(re, m->selected == QUITM_EXIT, x, y, w, h, "YES");
}

static enum menu_result quitmenu_action(void* userdata, enum action action)
{
	struct quitmenu_state* m = userdata;

	switch (action) {
		case ACT_UP:
			if (m->selected != QUITM_DONTEXIT) {
				m->selected--;
				play_menu_move();
			}
			return MNU_NONE;
		case ACT_DOWN:
			if (m->selected != QUITM_EXIT) {
				m->selected++;
				play_menu_move();
			}
			return MNU_NONE;
		case ACT_BACK:
			return MNU_BACK;
		case ACT_CONFIRM:
			switch (m->selected) {
				case QUITM_DONTEXIT:
					play_menu_confirm();
					return MNU_BACK;
				case QUITM_EXIT:
					play_menu_confirm();
					return MNU_QUIT;
			}
		default:
			return MNU_NONE;
	}
}

void destroy_quitmenu(struct menu* m)
{
	if (m != NULL && m->userdata != NULL) free(m->userdata);
}

int create_quitmenu(struct menu* m)
{
	assert(m != NULL);

	struct quitmenu_state* state = malloc(sizeof(struct quitmenu_state));

	if (state == NULL)
		return 0;

	state->selected = QUITM_DONTEXIT;
	
	m->userdata = state;
	m->paint = &quitmenu_paint;
	m->action = &quitmenu_action;
	return 1;
}
