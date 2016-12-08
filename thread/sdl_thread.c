#include "thread.h"


thread* th_create(int (*start)(void*), const char* name, void* arg)
{
	return SDL_CreateThread(start, name, arg);
}

void th_detach(thread* t)
{
	SDL_DetachThread(t);
}

void th_wait(thread* t, int* r)
{
	SDL_WaitThread(t, r);
}

thread_id th_current_id() {
	return SDL_ThreadID();
}

thread_id th_get_id(thread* t)
{
	return SDL_GetThreadID(t);
}

const char* th_get_name(thread* t)
{
	return SDL_GetThreadName(t);
}

int th_set_current_prio(enum thread_priority p)
{
	SDL_ThreadPriority prio;

	switch(p) {
		case THREAD_PRIO_LOW:
			prio = SDL_THREAD_PRIORITY_LOW;
			break;
		case THREAD_PRIO_NORMAL:
			prio = SDL_THREAD_PRIORITY_NORMAL;
			break;
		case THREAD_PRIO_HIGH:
			prio = SDL_THREAD_PRIORITY_HIGH;
			break;
		default:
			return -1;
	}

	return SDL_SetThreadPriority(prio);
}

thread_local th_local_create()
{
	return SDL_TLSCreate();
}

void* th_local_get(thread_local l)
{
	return SDL_TLSGet(l);
}

int th_local_set(thread_local l, const void* d, void (*destroy)(void*))
{
	return SDL_TLSSet(l, d, destroy);
}


