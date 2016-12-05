#include "thread/mutex.h"

#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

int mutex_unlock(mutex* m)
{
	assert(m != NULL);

	if (SDL_mutexV(m)==-1)
		return 0;
	else
		return 1;
}

int mutex_lock(mutex* m)
{
	assert(m != NULL);

	if (SDL_mutexP(m) == -1)
		return 0;
	else
		return 1;
}

void mutex_free(mutex* m)
{
	assert(m != NULL);

	SDL_DestroyMutex(m);
}

mutex* mutex_create()
{
	return SDL_CreateMutex();
}
