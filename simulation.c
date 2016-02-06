#include "simulation.h"

#include <stdlib.h>
#include <stdio.h>

#include "audioproxy.h"

#define RECT_INTERSECTS(x1, y1, w1, h1, x2, y2, w2, h2) !(x1 + w1 < x2 \
                                                       || x2 + w2 < x1 \
                                                       || y1 + h1 < y2 \
                                                       || y2 + h2 < y1)

void step_simulation(struct scene* s)
{
	s->b.x += s->b.dx;
	s->b.y += s->b.dy;

	if (s->p1.d == 1) {
		if (s->p1.y + s->p1.width < s->h) s->p1.y += s->p1.s;
	} else if (s->p1.d == -1) {
		if (s->p1.y > 0) s->p1.y -= s->p1.s;
	}

	if (s->p2.d == 1) {
		if (s->p2.y + s->p2.width < s->h) s->p2.y += s->p2.s;
	} else if (s->p2.d == -1) {
		if (s->p2.y >0) s->p2.y -= s->p2.s;
	}
	
	
	if (RECT_INTERSECTS(s->b.x, s->b.y, BALLSIZE, BALLSIZE,
	                    s->p1.x, s->p1.y, PADDLETHICKNESS, s->p1.width)) {
		// P1 has the ball
		s->b.dx = -s->b.dx * BOUNCESPEEDSCALE;

		// Normalized Y on paddle
		real py = s->p1.y + s->p1.width / 2;
		real by = s->b.y + BALLSIZE/2;

		real diffnorm = (by - py) / (s->p1.width/2);

		real perc = YCOORDACCELL;
		if ((diffnorm < 0 && s->p2.d < 0)
		 || (diffnorm > 0 && s->p2.d > 0))
			perc = YCOORDACCELLSAME;

		s->b.dy += (diffnorm * perc) * s->b.dx;

		play_paddle_hit();
	}

	if (RECT_INTERSECTS(s->b.x, s->b.y, BALLSIZE, BALLSIZE,
	                    s->p2.x, s->p2.y, PADDLETHICKNESS, s->p2.width)) {
		// P2 has the ball
		s->b.dx = -s->b.dx * BOUNCESPEEDSCALE;
		
		// Normalized Y on paddle
		real py = s->p2.y + s->p2.width / 2;
		real by = s->b.y + BALLSIZE/2;

		real diffnorm = (by - py) / (s->p2.width/2);

		real perc = YCOORDACCELL;
		if ((diffnorm < 0 && s->p1.d < 0)
		 || (diffnorm > 0 && s->p1.d > 0))
			perc = YCOORDACCELLSAME;

		s->b.dy += (diffnorm * perc) * s->b.dx;

		play_paddle_hit();
	}

	if (s->b.y + BALLSIZE > s->h || s->b.y < 0)
		s->b.dy = -s->b.dy;

	if (s->b.x < 0) {
		// Player 2 point
		s->p2pt++;

		s->b.x = s->w / 2;
		s->b.y = s->h / 2;
		s->b.dy = 0;
		s->b.dx = s->bs;
		s->bs += BALLSPEEDINCREASE;

		play_player_point();
	}
	if (s->b.x > s->w) {
		// Player 1 point
		s->p1pt++;

		s->b.x = s->w / 2;
		s->b.y = s->h / 2;
		s->b.dy = 0;
		s->b.dx = -s->bs;
		s->bs += BALLSPEEDINCREASE;

		play_player_point();
	}

	if (s->p1pt >= WINSTATE && s->p2pt >= WINSTATE) {
		printf("It's a tie!");
		exit(0);
	} else if (s->p1pt >= WINSTATE) {
		printf("Player 1 won!");
		exit(0);
	} else if (s->p1pt >= WINSTATE) {
		printf("Player 2 won!");
		exit(0);
	}
}