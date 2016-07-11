#ifndef RENDER_H
#define RENDER_H

#include <stddef.h>

struct renderer;

struct render_item;

enum render_text_alignment {
	RTA_LEFT,
	RTA_RIGHT,
};

size_t ri_drawtext(struct renderer* r,
                   enum render_text_alignment a,
                   int x, int y,
                   int w, int h,
                   const char* text);

int ri_outputbounds(struct renderer* r, int* w, int* h);

void ri_draw(struct renderer* r,
             struct render_item* i,
             int x, int y,
             int w, int h);

void ri_clear(struct renderer* r);
void ri_flip(struct renderer* r);

struct render_item* ri_load(struct renderer* r, const char* path);
void ri_destroy(struct renderer* r, struct render_item*);

#endif /* RENDER_H */
