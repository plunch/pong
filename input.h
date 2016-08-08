#ifndef INPUT_H
#define INPUT_H

#include "typedefs.h"
#include "gameaction.h"

struct input {
	real input[GAMEACTION_MAX];
};

#define I_P(inp, ga) ((inp)->input[ga] > 0.99)
#define I_N(inp, ga) ((inp)->input[ga] < -0.99)

#define I_MAX (1.0)
#define I_MIN (-1.0)
#define I_ZERO (0.0)

#endif /* INPUT_H */
