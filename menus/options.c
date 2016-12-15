#include "options.h"

#include <math.h>
#include <limits.h>
#include <assert.h>
#include "../global.h"
#include "render.h"

#include "common.h"

#include "../audioproxy.h"

#define VOLINC 0.05f

enum options_option {
	OPT_VOLUME,
	OPT_KEYBIND,
	OPT_SAVE,
	OPT_BACK,
};

struct optionsmenu_state {
	struct render_item* slider;
	struct render_item* sliderbox;
	float orig_volume, before_muted;
	int ismuted;
	enum options_option selected;
};

static void optionsmenu_paint(void* userdata, struct renderer* re)
{
	struct optionsmenu_state* s = userdata;
	UNUSED(s);

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
		// Center horizontally
		y = 0;
		x = rw / 2 - w / 2;
	} else {
		// Align to top
		y = 0;
		x = 0;
	}

	y = (h/6) - 50/2;
	ri_drawtext(re, RTA_LEFT, RTS_DEFAULT, x, y, w, 50, "OPTIONS");

	y = h/3;


	long int roundresult = lrintf(((float)w-40) * proxy_getvolume());
	assert(roundresult < INT_MAX && roundresult > INT_MIN);

	if (s->selected == OPT_VOLUME)
		ri_draw(re, s->sliderbox, x, y, w, 50);
	if (!s->ismuted)
		ri_draw(re, s->slider, x + 20, y + 20, (int)roundresult, 10);

	y += 50;
	y += 10;

	y += draw_option(re, s->selected == OPT_KEYBIND,
	                 x, y, w, h, "INPUT");
	y += 50;
	y += draw_option(re, s->selected == OPT_SAVE,
	                 x, y, w, h, "ACCEPT");
	y += draw_option(re, s->selected == OPT_BACK,
	                 x, y, w, h, "BACK");
}

static enum menu_result optionsmenu_action(void* userdata, enum action action)
{
	struct optionsmenu_state* s = userdata;

	switch (action) {
		case ACT_UP:
			if (s->selected != OPT_VOLUME) {
				s->selected--;
				play_menu_move();
			}
			return MNU_NONE;
		case ACT_DOWN:
			if (s->selected != OPT_BACK) {
				s->selected++;
				play_menu_move();
			}
			return MNU_NONE;
		case ACT_LEFT:
			if (s->selected == OPT_VOLUME) {
				float f = proxy_getvolume();
				f -= VOLINC;
				if (f < 0.0f) f = 0.0f;
				proxy_setvolume(f);
				play_menu_move();
			}
			return MNU_NONE;
		case ACT_RIGHT:
			if (s->selected == OPT_VOLUME) {
				float f = proxy_getvolume();
				f += VOLINC;
				if (f > 1.0f) f = 1.0f;
				proxy_setvolume(f);
				play_menu_move();
			}
			return MNU_NONE;
		case ACT_CONFIRM:
			switch(s->selected) {
				case OPT_VOLUME:
					s->ismuted = !s->ismuted;
					if (s->ismuted) {
						s->before_muted = proxy_getvolume();
						proxy_setvolume(0);
					} else {
						proxy_setvolume(s->before_muted);
					}
					play_menu_move();
					return MNU_NONE;
				case OPT_KEYBIND:
					return MNU_NONE;
				case OPT_BACK:
					play_menu_back();
					proxy_setvolume(s->orig_volume);
					return MNU_BACK;
				case OPT_SAVE:
					play_menu_confirm();
					return MNU_FORWARD;
			}
		case ACT_BACK:
			// TODO: are you sure
			play_menu_back();
			proxy_setvolume(s->orig_volume);
			return MNU_BACK;
		default:
			return MNU_NONE;
	}
}

void destroy_optionsmenu(struct menu* m)
{
	if (m->userdata != NULL) {
		struct optionsmenu_state* s = m->userdata;
		ri_destroy(m->renderer, s->slider);
		ri_destroy(m->renderer, s->sliderbox);
		free(s);
	}
}

int create_optionsmenu(struct menu* m)
{
	struct optionsmenu_state* state = malloc(sizeof(struct optionsmenu_state));
	if (state == NULL)
		return 0;

	state->orig_volume = proxy_getvolume();
	state->selected = OPT_VOLUME;

	state->slider = ri_load(m->renderer, "volumeslider");
	state->sliderbox = ri_load(m->renderer, "volumesliderbox");

	m->userdata = state;
	m->paint = &optionsmenu_paint;
	m->action = &optionsmenu_action;
	return 1;
}
