#ifndef INPUT_H
#define INPUT_H

#include "typedefs.h"

// Contains all possible game actions that a player
// can take.
enum game_action {
	GA_P1_DOWN,
	GA_P2_DOWN,
	GA_P1_UP,
	GA_P2_UP,

	GAMEACTION_MAX,
};

struct input {
	real input[GAMEACTION_MAX];
};

#endif /* INPUT_H */
