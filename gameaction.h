#ifndef GAMEACTION_H
#define GAMEACTION_H


#define GAMEACTIONLIST \
	X(P1_MOVE_UP) \
	X(P1_MOVE_DOWN) \
	X(P2_MOVE_UP) \
	X(P2_MOVE_DOWN)

#define X(x) GA_##x,
enum game_action {
	GAMEACTIONLIST

	GAMEACTION_MAX
};
#undef X

const char* game_action_names[GAMEACTION_MAX];

#endif /* GAMEACTION_H */
