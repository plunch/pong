#ifndef THREAD_MUTEX_H
#define THREAD_MUTEX_H

#include <SDL2/SDL_thread.h>

typedef SDL_mutex mutex;

int mutex_unlock(mutex*);
int mutex_lock(mutex*);

void mutex_free(mutex*);
mutex* mutex_create();

#endif /* THREAD_MUTEX_H */
