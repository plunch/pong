#include "global.h"
#include "sdl_renderer.h"
#include "renderimpl.h"
#include <assert.h>
#include <string.h>

#define BGCOLOR 0x22, 0x69, 0x88
#define FGCOLOR 0xD1, 0x78, 0x86

#define ITEM_TABLE \
	X("paddle1", 0xD1, 0x78, 0x86, 1) \
	X("paddle2", 0xD1, 0x78, 0x86, 1) \
	X("ball", 0xD1, 0x78, 0x86, 1) \
	X("field", 0xD1, 0x78, 0x86, 4) \
	X("volumeslider", 0xD1, 0x78, 0x86, 3) \
	X("volumesliderbox", 0xD1, 0x78, 0x86, 0) \
	X("winstategfx", 0xD1, 0x78, 0x86, 1) \

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

	SDL_Rect r, top, bottom, left, right;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;

	switch(item->fill) {
		case 1:
			SDL_RenderFillRect(re->rend, &r);
			break;
		case 2:
			top.x = r.x + r.w/2;
			top.y = r.y;
			top.w = 10;
			top.h = r.h;
			SDL_RenderFillRect(re->rend, &top);

			// FALLTHROUGH
		case 0:
			top.x = r.x;
			top.y = r.y;
			top.w = r.w;
			top.h = 10;

			bottom.x = r.x;
			bottom.y = r.y + r.h - 10;
			bottom.w = r.w;
			bottom.h = 10;

			left.x = r.x;
			left.y = r.y;
			left.w = 10;
			left.h = r.h;

			right.x = r.x + r.w - 10;
			right.y = r.y;
			right.w = 10;
			right.h = r.h;

			SDL_RenderFillRect(re->rend, &top);
			SDL_RenderFillRect(re->rend, &bottom);
			SDL_RenderFillRect(re->rend, &left);
			SDL_RenderFillRect(re->rend, &right);
			break;
		case 3:
			top.x = r.x;
			top.y = r.y;
			top.w = 10;
			top.h = r.h;

			while(top.x + top.w < r.x + r.w) {
				SDL_RenderFillRect(re->rend, &top);
				top.x += top.w * 2;
			}
		case 4:
			top.x = r.x + r.w/2;
			top.y = r.y;
			top.w = 10;
			top.h = 10;

			while(top.y + top.h < r.y + r.h) {
				SDL_RenderFillRect(re->rend, &top);
				top.y += top.h * 2;
			}

			top.x = r.x;
			top.y = r.y;
			top.w = r.w;
			top.h = 10;

			bottom.x = r.x;
			bottom.y = r.y + r.h - 10;
			bottom.w = r.w;
			bottom.h = 10;

			SDL_RenderFillRect(re->rend, &top);
			SDL_RenderFillRect(re->rend, &bottom);
			break;
	}
}

static size_t drawtext(void* data, 
                       enum render_text_alignment a,
                       enum render_text_style s,
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
	
	if (s & RTS_SELECTED) {
		SDL_SetRenderDrawColor(re->rend, FGCOLOR, SDL_ALPHA_OPAQUE);
		region.x -= 10;
		region.y -= 10;
		region.w += 20;
		region.h += 20;
		SDL_RenderFillRect(re->rend, &region);
		SDL_SetTextureColorMod(re->text->b, BGCOLOR);
		region.x += 10;
		region.y += 10;
		region.w -= 20;
		region.h -= 20;
	}
	size_t ret = render_text(re->rend, re->text, region, align, text);
	if (s & RTS_SELECTED) SDL_SetTextureColorMod(re->text->b, FGCOLOR);
	return ret;
}

static void clear(void* data)
{
	struct sdl_renderer* re = data;
	if (SDL_SetRenderDrawColor(re->rend, BGCOLOR, SDL_ALPHA_OPAQUE) != 0) {
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

void destroy_sdl_renderer(struct renderer* rend)
{
	if (rend != NULL && rend->userdata != NULL)
		free(rend->userdata);
}
