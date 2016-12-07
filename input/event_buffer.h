#ifndef EVENT_BUFFER_H
#define EVENT_BUFFER_H

#include "input/event.h"
#include "thread/mutex.h"

#ifndef INPUT_EVENT_BUFZ
#define INPUT_EVENT_BUFZ 1024
#endif

struct input_event_buffer {
	struct input_event events[INPUT_EVENT_BUFZ];
	size_t start, size;
	mutex* mut;
};

int input_event_buffer_init(struct input_event_buffer*);
void input_event_buffer_free(struct input_event_buffer*);

int input_event_buffer_push(struct input_event_buffer*, struct input_event*);
int input_event_buffer_pop(struct input_event_buffer*, struct input_event*);	
int input_event_buffer_peek(struct input_event_buffer*, struct input_event*);
int input_event_buffer_popif(struct input_event_buffer*,
		             struct input_event*,
			     void*,
			     int (*predicate)(void*, struct input_event*));
size_t input_event_buffer_popwhile(struct input_event_buffer*,
		                   struct input_event*,
				   size_t,
				   void*,
			           int (*predicate)(void*, struct input_event*));

#endif /* EVENT_BUFFER_H */
