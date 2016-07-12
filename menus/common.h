#ifndef MENU_COMMON_H
#define MENU_COMMON_H

#include "global.h"
#include "render.h"

static int draw_option(struct renderer* re, int isSelected,
                       int x, int y, int w, int h,
                       const char* text)
{
	if (h > 50)
		h = 50;

	ri_drawtext(re, RTA_LEFT, isSelected ? RTS_SELECTED : RTS_DEFAULT, x, y, w, h, text);
	return h;
}

#endif /* MENU_COMMON_H */
