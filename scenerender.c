#include "scenerender.h"
#include <SDL2/SDL.h>

#include <stdio.h>
#include <string.h>


#include "render.h"

#define NUMPXSIZE 10

static int rintit(real v)
{
	return (int)(rint(v));
}

static void print_score(struct renderer* re, int rx, int ry, int rw, int rh,
                        unsigned int pt, int left)
{
	char strpt[5] = {'\0'};
	snprintf(strpt, 5, "%u", pt);

	size_t l = strlen(strpt);
	if (!l) return;

	if (left) {
		int x, y;
		x = rx;
		y = ry;

		ri_drawtext(re, RTA_LEFT, RTS_DEFAULT, x, y, rw, rh, strpt);
	} else {
		int x, y;
		x = rx + rw - NUMPXSIZE * 3;
		y = ry;

		ri_drawtext(re, RTA_RIGHT, RTS_DEFAULT, x, y, rw, rh, strpt);
	}
}

void draw_scene(struct scene* s,
                struct renderer* re,
                struct render_item* paddle1,
                struct render_item* paddle2,
                struct render_item* ball,
                struct render_item* field)
{
	int w, h;
	ri_outputbounds(re, &w, &h);

	// Field
	
	ri_draw(re, field, 0, 0, rintit(s->w), rintit(s->h));


	// Ball

	ri_draw(re, ball, 
	        rintit(s->b.x),
	        rintit(s->b.y),
	        BALLSIZE, BALLSIZE);

	
	// Player 1

	ri_draw(re, paddle1,
	        rintit(s->p1.x),
	        rintit(s->p1.y),
	        PADDLETHICKNESS,
	        rintit(s->p1.width));


	// Player 2

	ri_draw(re, paddle2,
	        rintit(s->p2.x),
	        rintit(s->p2.y),
	        PADDLETHICKNESS,
	        rintit(s->p2.width));

	int tx = 30;
	int ty = 30;
	int tw = rintit(s->w / 2.0 - 60);
	int th = NUMPXSIZE * 5;

	print_score(re, tx, ty, tw, th, s->p1pt, 0);

	
	tx = rintit(s->w / 2.0 + 30.0);
	print_score(re, tx, ty, tw, th, s->p2pt, 1);
}
