#include "mainmenu.h"

#include "common.h"

#include "../audioproxy.h"

#include <stdlib.h>

enum mainmenu_option {
	MAINM_PLAY = 0,

	MAINM_OPTIONS = 1,

	MAINM_QUIT = 2,
};

struct mainmenu_state {
	enum mainmenu_option selected;
};


static void mainmenu_paint(void* userdata, struct renderer* r)
{
	struct mainmenu_state* m = userdata;

	int rw, rh, w, h;
	ri_outputbounds(r, &rw, &rh);

	if (rw > rh) {
		w = rh / 2;
		h = rh;
	} else {
		h = rw * 2;
		w = rw;
	}
	
	int x, y;
	if (w < rw) {
		// Center horizontally
		y = 0;
		x = rw / 2 - w / 2;
	} else {
		// Align to top
		y = 0;
		x = 0;
	}

	//int logoh = h / 3;

	y = (h/6) - 50/2;

	// Draw logo
	ri_drawtext(r, RTA_LEFT, RTS_DEFAULT, x, y, w, 50, "PONG");
	
	int opty = h/3;
	// Draw x
	opty += draw_option(r, m->selected == MAINM_PLAY, x, opty, w, h, "PLAY");
	opty += draw_option(r, m->selected == MAINM_OPTIONS, x, opty, w, h, "OPTIONS");
	opty += draw_option(r, m->selected == MAINM_QUIT, x, opty, w, h, "QUIT");
}

static enum menu_result mainmenu_action(void* userdata, enum action action,
                                        int x, int y)
{
	(void)x;
	(void)y;
	struct mainmenu_state* m = userdata;

	switch (action) {
		case ACT_UP:
			if (m->selected != MAINM_PLAY) {
				m->selected--;
				play_menu_move();
			}
			return MNU_NONE;
		case ACT_DOWN:
			if (m->selected != MAINM_QUIT) {
				m->selected++;
				play_menu_move();
			}
			return MNU_NONE;
		case ACT_BACK:
			return MNU_QUIT;
		case ACT_CONFIRM:
			switch (m->selected) {
				case MAINM_PLAY:
					play_menu_confirm();
					return MNU_FORWARD;
				case MAINM_QUIT:
					play_menu_confirm();
					return MNU_QUIT;
				case MAINM_OPTIONS:
					play_menu_confirm();
					return MNU_OPT1;
			}
		default:
			return MNU_NONE;
	}
}

void destroy_mainmenu(struct menu* m)
{
	if (m->userdata != NULL) free(m->userdata);
}

int create_mainmenu(struct menu* m)
{
	if (m == NULL) 
		return 0;

	struct mainmenu_state* state = malloc(sizeof(struct mainmenu_state));
	if (state == NULL)
		return 0;

	state->selected = MAINM_PLAY;

	m->userdata = state;
	m->paint = &mainmenu_paint;
	m->action = &mainmenu_action;
	return 1;
}
