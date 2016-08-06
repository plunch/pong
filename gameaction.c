#include "gameaction.h"

#define X(x) #x,
const char* game_action_names[GAMEACTION_MAX] = {
	GAMEACTIONLIST
};
#undef X
