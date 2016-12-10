#ifndef SIM_H
#define SIM_H

#include "typedefs.h"

#define WINSTATE 13
#define YCOORDACCELL 0.5
#define YCOORDACCELLSAME 0.75
#define BALLSPEEDINCREASE 4
#define BOUNCESPEEDSCALE 1.2

#define BALLSIZE 20

struct ball {
	real x, y;
	real dx, dy;
};

#define PADDLETHICKNESS 50

struct paddle {
	real x, y;
	real width;
	real s;
	int d : 2;
};

struct scene {
	real w, h;
	unsigned int p1pt, p2pt;
	real bs;
	struct ball b;
	struct paddle p1, p2;
};

int step_simulation(struct scene*, real);

#endif /* SIM_H */
