#include "mainmenu.h"

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

static int draw_option(SDL_Renderer* re, 
                       struct mainmenu_state* m,
		       enum mainmenu_option opt,
                       int x, int y,
                       int w, int h)
{
	if (h > 50)
		h = 50;

	SDL_Rect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;

	SDL_SetRenderDrawColor(re, 255, 255, 255, SDL_ALPHA_OPAQUE);
	
	if (m->selected == opt) {
		SDL_RenderFillRect(re, &r);
	} else {
		SDL_RenderDrawRect(re, &r);
	}

	return h;
}

static void mainmenu_paint(void* userdata, SDL_Renderer* r)
{
	struct mainmenu_state* m = userdata;

	int rw, rh, w, h;
	SDL_GetRendererOutputSize(r, &rw, &rh);

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

	int logoh = h / 3;

	// Draw logo
	
	int opty = y + logoh;
	// Draw x
	opty += draw_option(r, m, MAINM_PLAY, x, opty, w, h);
	opty += draw_option(r, m, MAINM_OPTIONS, x, opty, w, h);
	opty += draw_option(r, m, MAINM_QUIT, x, opty, w, h);
}

static enum menu_result mainmenu_action(void* userdata, enum action action)
{
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
