#include "options.h"

#include <math.h>
#include "../global.h"

#include "../audioproxy.h"

#define VOLINC 0.05f

enum options_option {
	OPT_VOLUME,
	OPT_SAVE,
	OPT_BACK,
};

struct optionsmenu_state {
	float orig_volume;
	enum options_option selected;
};

static void optionsmenu_paint(void* userdata, SDL_Renderer* re)
{
	struct optionsmenu_state* s = userdata;
	UNUSED(s);

	int w, h;
	SDL_GetRendererOutputSize(re, &w, &h);

	SDL_Rect r;
	r.x = 0;
	r.y = 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbad-function-cast"
	r.w = (int)rintf((float)w * proxy_getvolume());
	r.h = h;
#pragma GCC diagnostic pop

	SDL_SetRenderDrawColor(re, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(re, &r);
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
				if (f > 1.0f) f = 1.0f;
				proxy_setvolume(f);
				play_menu_move();
			}
			return MNU_NONE;
		case ACT_RIGHT:
			if (s->selected == OPT_VOLUME) {
				float f = proxy_getvolume();
				f += VOLINC;
				if (f < 0.0f) f = 0.0f;
				proxy_setvolume(f);
				play_menu_move();
			}
			return MNU_NONE;
		case ACT_CONFIRM:
			play_menu_confirm();
			return MNU_FORWARD;
		case ACT_BACK:
			// TODO: are you sure
			play_menu_back();
			proxy_setvolume(s->orig_volume);
			return MNU_BACK;
		default:
			return MNU_NONE;
	}
}

int create_optionsmenu(struct menu* m)
{
	struct optionsmenu_state* state = malloc(sizeof(struct optionsmenu_state));
	if (state == NULL)
		return 0;

	state->orig_volume = proxy_getvolume();

	m->userdata = state;
	m->paint = &optionsmenu_paint;
	m->action = &optionsmenu_action;
	return 1;
}
