#include "scenerender.h"

#include <stdio.h>
#include <string.h>

#include "numbers.h"

#define NUMPXSIZE 10

static int rintit(real v)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbad-function-cast"
	return (int)rint(v);
#pragma GCC diagnostic pop
}

static void print_scorechar(SDL_Renderer* rend, char c, int x, int y)
{
	SDL_Rect re;
	re.x = x;
	re.y = y;
	re.w = NUMPXSIZE;
	re.h = NUMPXSIZE;

	const char (*num)[5][3];

	switch(c) {
		case '0':
			num = &num0;
			break;
		case '1':
			num = &num1;
			break;
		case '2':
			num = &num2;
			break;
		case '3':
			num = &num3;
			break;
		case '4':
			num = &num4;
			break;
		case '5':
			num = &num5;
			break;
		case '6':
			num = &num6;
			break;
		case '7':
			num = &num7;
			break;
		case '8':
			num = &num8;
			break;
		case '9':
			num = &num9;
			break;
		default:
			return;
	}

	for(int col = 0; col < 3; ++col) {
		for(int row = 0; row < 5; ++row) {
			if ((*num)[row][col]) {
				SDL_RenderFillRect(rend, &re);
			}
			re.y += NUMPXSIZE;
		}
		re.x += NUMPXSIZE;
		re.y -= NUMPXSIZE * 5;
	}
}

static void print_score(SDL_Renderer* re, SDL_Rect* r,
                        unsigned int pt, int left)
{
	char strpt[5] = {'\0'};
	snprintf(strpt, 5, "%u", pt);

	size_t l = strlen(strpt);
	if (!l) return;

	if (left) {
		int x, y;
		x = r->x;
		y = r->y;

		for(size_t i = 0; i < l; ++i) {
			print_scorechar(re, strpt[i], x, y);
			x += NUMPXSIZE * 4;
		}
	} else {
		int x, y;
		x = r->x + r->w - NUMPXSIZE * 3;
		y = r->y;

		for(size_t i = l; i >= 1; --i) {
			print_scorechar(re, strpt[i-1], x, y);
			x -= NUMPXSIZE * 4;
		}
	}
}

void draw_scene(struct scene* s, SDL_Renderer* re)
{
	SDL_SetRenderDrawColor(re, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(re);

	int w, h;
	SDL_GetRendererOutputSize(re, &w, &h);

	// Center
	SDL_SetRenderDrawColor(re, 190, 190, 190, SDL_ALPHA_OPAQUE);

	SDL_RenderDrawLine(re, rintit(s->w / 2), 0, rintit(s->w / 2), rintit(s->h));


	SDL_Rect r;
	r.x = 0; r.y = 0;
	r.w = rintit(s->w); r.h = rintit(s->h);
	SDL_RenderDrawRect(re, &r);



	SDL_SetRenderDrawColor(re, 255, 255, 255, SDL_ALPHA_OPAQUE);


	// Ball

	r.x = rintit(s->b.x);
	r.y = rintit(s->b.y);
	r.w = BALLSIZE;
	r.h = BALLSIZE;

	SDL_RenderFillRect(re, &r);

	
	// Player 1

	r.x = rintit(s->p1.x);
	r.y = rintit(s->p1.y);
	r.w = PADDLETHICKNESS;
	r.h = rintit(s->p1.width);

	SDL_RenderFillRect(re, &r);


	// Player 1
	//
	r.x = rintit(s->p2.x);
	r.y = rintit(s->p2.y);
	r.h = rintit(s->p2.width);

	SDL_RenderFillRect(re, &r);

	r.x = 30;
	r.y = 30;
	r.w = rintit(s->w / 2.0 - 60);
	r.h = NUMPXSIZE * 5;

	print_score(re, &r, s->p1pt, 0);

	r.x = rintit(s->w / 2.0 + 30.0);
	print_score(re, &r, s->p2pt, 1);
}
