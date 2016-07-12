#include "render.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "renderimpl.h"

static void uiz(const char* what)
{
	fprintf(stderr, "Renderer: %s uninitialized\n", what);
}


size_t ri_drawtext(struct renderer* r,
                   enum render_text_alignment a,
		   enum render_text_style s,
                   int x, int y,
                   int w, int h,
                   const char* text)
{
	if (r == NULL || r->drawtext == NULL) {
		uiz("drawtext");
		return 0;
	}
	assert(text != NULL);

	return r->drawtext(r->userdata, a, s, x, y, w, h, text);
}

int ri_outputbounds(struct renderer* r, int* w, int* h)
{
	if (r == NULL || r->outputbounds == NULL) {
		uiz("outputbounds");
		if (w) w = 0;
		if (h) h = 0;
		return 0;
	}
	return r->outputbounds(r->userdata, w, h);
}

void ri_draw(struct renderer* r,
             struct render_item* item,
             int x, int y,
             int w, int h)
{
	if (r == NULL || r->draw == NULL) {
		uiz("draw");
	}

	r->draw(r->userdata, item, x, y, w, h);
}

void ri_clear(struct renderer* r)
{
	if (r != NULL && r->clear != NULL) {
		r->clear(r->userdata);
	} else {
		uiz("clear");
	}
}
void ri_flip(struct renderer* r)
{
	if (r != NULL && r->flip != NULL) {
		r->flip(r->userdata);
	} else {
		uiz("flip");
	}
}

struct render_item* ri_load(struct renderer* r, const char* path)
{
	assert(path != NULL);
	if (r == NULL || r->load == NULL) {
		uiz("load");
		return NULL;
	}

	return r->load(r->userdata, path);
}
void ri_destroy(struct renderer* r, struct render_item* ri)
{
	if (r == NULL || r->destroy == NULL) {
		uiz("destroy");
		return;
	}
	r->destroy(r->userdata, ri);
}

