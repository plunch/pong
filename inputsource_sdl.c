#include "inputsource_sdl.h"

void inputsource_apply(struct inputsourcelist* list, struct input* in, SDL_Event* e)
{
	for(size_t i = 0; i < list->len; ++i) {
		if (list->sources[i].apply != NULL) {
			void * data = list->sources[i].data;
			list->sources[i].apply(data, in, e);
		}
	}
}
