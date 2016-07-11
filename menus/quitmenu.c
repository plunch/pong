#include "quitmenu.h"
#include <assert.h>

#include "global.h"
#include "menuimpl.h"
#include "audioproxy.h"

enum quitmenu_option {
	QUITM_DONTEXIT = 0,
	QUITM_EXIT = 1,
};

struct quitmenu_state {
	enum quitmenu_option selected;
};

static int drawopt(SDL_Renderer* re,
                   struct quitmenu_state* m,
                   enum quitmenu_option opt,
                   int x, int y,
                   int w, int h,
                   struct textinfo* ti,
                   const char* text)
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

		SDL_SetTextureColorMod(ti->b, 0, 0, 0);
		render_text(re, ti, r, text);
		SDL_SetTextureColorMod(ti->b, 255, 255, 255);
	} else {
		render_text(re, ti, r, text);
	}

	return h;
}

static void quitmenu_paint(void* userdata, SDL_Renderer* re, struct textinfo* ti)
{
	int rw, rh, w, h;
	SDL_GetRendererOutputSize(re, &rw, &rh);

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
	SDL_Rect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = 50;
	render_text(re, ti, r, "QUIT NOW?");

	y = h / 3;

	y += drawopt(re, userdata, QUITM_DONTEXIT, x, y, w, h, ti, "NO");
	y += drawopt(re, userdata, QUITM_EXIT, x, y, w, h, ti, "YES");
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
			return MNU_NONE;
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
