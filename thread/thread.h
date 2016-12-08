#ifndef THREAD_THREAD_H
#define THREAD_THREAD_H

#include <SDL2/SDL_thread.h>

typedef SDL_Thread thread;
typedef SDL_TLSID thread_local;
typedef SDL_threadID thread_id;

thread* th_create(int (*fn)(void*), const char*, void*);

void th_detach(thread*);

void th_wait(thread*, int*);

thread_id th_current_id(void);
thread_id th_get_id(thread*);

const char* th_get_name(thread*);

enum thread_priority {
	THREAD_PRIO_LOW,
	THREAD_PRIO_NORMAL,
	THREAD_PRIO_HIGH,
};

int th_set_current_prio(enum thread_priority);

thread_local th_local_create(void);
void* th_local_get(thread_local);
int th_local_set(thread_local, const void*, void (*destroy)(void*));

#endif /* THREAD_THREAD_H */
