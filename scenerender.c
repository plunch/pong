#include "scenerender.h"
#include <SDL2/SDL.h>

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>


#include "render.h"

#define NUMPXSIZE 10

static int round2integer(real v)
{
	long int roundresult = lrint_real(v);
	assert(roundresult < INT_MAX && roundresult > INT_MIN);
	return (int)roundresult;
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
	
	ri_draw(re, field,
	        round2integer(s->offx),
	        round2integer(s->offy),
	        round2integer(s->w),
	        round2integer(s->h));


	// Ball

	ri_draw(re, ball, 
	        round2integer(s->offx + s->b.x),
	        round2integer(s->offy + s->b.y),
	        BALLSIZE, BALLSIZE);

	
	// Player 1

	ri_draw(re, paddle1,
	        round2integer(s->offx + s->p1.x),
	        round2integer(s->offy + s->p1.y),
	        PADDLETHICKNESS,
	        round2integer(s->p1.width));


	// Player 2

	ri_draw(re, paddle2,
	        round2integer(s->offx + s->p2.x),
	        round2integer(s->offy + s->p2.y),
	        PADDLETHICKNESS,
	        round2integer(s->p2.width));

	int tx = round2integer(s->offx + 30);
	int ty = round2integer(s->offy + 30);
	int tw = round2integer(s->w / 2.0 - 60);
	int th = NUMPXSIZE * 5;

	print_score(re, tx, ty, tw, th, s->p1pt, 0);

	
	tx = round2integer(s->w / 2.0 + 30.0 + s->offx);
	print_score(re, tx, ty, tw, th, s->p2pt, 1);
}
