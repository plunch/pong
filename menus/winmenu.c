#include "winmenu.h"

#include "../global.h"

struct winmenu_state {
	int winner;
};

static void winmenu_paint(void* userdata, SDL_Renderer* re, struct textinfo* ti)
{
	struct winmenu_state* s = userdata;

	int w, h;
	SDL_GetRendererOutputSize(re, &w, &h);

	SDL_Rect r;
	r.x = 0;
	r.y = 0;
	r.w = w / 2;
	r.h = h;

	switch (s->winner) {
		default:
			r.w = w;
			break;
		case 1:
			break;
		case 2:
			r.x = w / 2;
			break;
	}

	SDL_SetRenderDrawColor(re, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(re, &r);
}

static enum menu_result winmenu_action(void* userdata, enum action action)
{
	UNUSED(userdata);
	
	if (action != ACT_NONE) 
		return MNU_FORWARD;
	else
		return MNU_NONE;
}

int create_winmenu(struct menu* m, int winner)
{
	struct winmenu_state* state = malloc(sizeof(struct winmenu_state));
	if (state == NULL)
		return 0;

	state->winner = winner;

	m->userdata = state;
	m->paint = &winmenu_paint;
	m->action = &winmenu_action;
	return 1;
}
