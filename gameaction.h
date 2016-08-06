#ifndef GAMEACTION_H
#define GAMEACTION_H


#define GAMEACTIONLIST \
	X(P1_MOVEMENT) \
	X(P2_MOVEMENT)

#define X(x) GA_##x,
enum game_action {
	GAMEACTIONLIST

	GAMEACTION_MAX
};
#undef X

const char* game_action_names[GAMEACTION_MAX];

#endif /* GAMEACTION_H */
