#include "global.h"
#include "sdl_renderer.h"
#include "renderimpl.h"
#include <assert.h>
#include <string.h>

#define ITEM_TABLE \
	X("paddle1", 255, 255, 255, 1) \
	X("paddle2", 255, 255, 255, 1) \
	X("ball", 255, 255, 255, 1) \
	X("field", 128, 128, 128, 0) \
	X("volumeslider", 255, 255, 255, 1)

static struct render_item* load(void* data, const char* path)
{
	UNUSED(data);
	struct render_item i;
	int searching = 1;
#define X(x, r, g, b, f) if (searching && strcmp(x, path) == 0) { \
	searching = 0; \
	struct render_item i2 = { r, g, b, f }; \
	i = i2; \
}
	ITEM_TABLE
#undef X
	
	// Not found if b is still set
	if (searching)
		return NULL;

	struct render_item* ret = malloc(sizeof(struct render_item));
	if (ret == NULL)
		return NULL;
	ret->r = i.r;
	ret->g = i.g;
	ret->b = i.b;
	ret->fill = i.fill;
	return ret;
}

static void destroy(void* data, struct render_item* item)
{
	UNUSED(data);
	if (item != NULL) free(item);
}

static void draw(void* data, struct render_item* item,
                 int x, int y, int w, int h)
{
	struct sdl_renderer* re = data;

	if (SDL_SetRenderDrawColor(re->rend, 
	                           item->r, 
	                           item->g,
	                           item->b,
	                           SDL_ALPHA_OPAQUE) != 0) {
		sdl_error("Set SDL_Renderer draw color before drawing item");
	}

	SDL_Rect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;

	if (item->fill) {
		SDL_RenderFillRect(re->rend, &r);
	} else {
		SDL_RenderDrawRect(re->rend, &r);
	}
}

static size_t drawtext(void* data, enum render_text_alignment a,
                       int x, int y, int w, int h,
                       const char* text)
{
	struct sdl_renderer* re = data;
	SDL_Rect region;
	region.x = x;
	region.y = y;
	region.w = w;
	region.h = h;
	enum text_alignment align;
	switch(a) {
		case RTA_RIGHT:
			align = TA_RIGHT;
			break;
		case RTA_LEFT:
			align = TA_LEFT;
			break;
		default:
			assert(0);
	}
	
	return render_text(re->rend, re->text, region, align, text);
}

static void clear(void* data)
{
	struct sdl_renderer* re = data;
	if (SDL_SetRenderDrawColor(re->rend, 0, 0, 0, SDL_ALPHA_OPAQUE) != 0) {
		sdl_error("Set SDL_Renderer draw color before clearing");
	}
	if (SDL_RenderClear(re->rend) != 0)
		sdl_error("Clear SDL_Renderer buffer");
}

static void flip(void* data)
{
	struct sdl_renderer* re = data;
	SDL_RenderPresent(re->rend);
}

static int bounds(void* data, int* w, int* h)
{
	struct sdl_renderer* re = data;
	if(SDL_GetRendererOutputSize(re->rend, w, h) == 0) {
		return 1;
	} else {
		sdl_error("Get SDL_Renderer output size");
		return 0;
	}
}

int create_sdl_renderer(struct renderer* rend, SDL_Window* w, SDL_Renderer* r, struct textinfo* ti)
{
	struct sdl_renderer* re = malloc(sizeof(struct sdl_renderer));
	if (re == NULL)
		return 0;

	re->wind = w;
	re->rend = r;
	re->text = ti;

	rend->userdata = re;
	rend->load = &load;
	rend->destroy = &destroy;
	rend->draw = &draw;
	rend->drawtext = &drawtext;
	rend->flip = &flip;
	rend->outputbounds = &bounds;
	rend->clear = & clear;
	return 0;
}
