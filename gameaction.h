#ifndef GAMEACTION_H
#define GAMEACTION_H


#define GAMEACTIONLIST \
	X(P1_DOWN) \
	X(P2_DOWN) \
	X(P1_UP) \
	X(P2_UP)

#define X(x) GA_##x,
enum game_action {
	GAMEACTIONLIST

	GAMEACTION_MAX
};
#undef X

#define X(x) #x,
static const char* game_action_names[GAMEACTION_MAX] = {
	GAMEACTIONLIST
};
#undef X

#endif /* GAMEACTION_H */
