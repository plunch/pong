#include "kernel.h"
#include "state.h"
#include "event_buffer.h"

#define BUFZ 512

static int event_is_before(void* arg, struct input_event* e)
{
	unsigned* ts = arg;
	return e->timestamp <= *ts;
}

void input_kernel_consume(struct input_kernel* kernel,
                          struct input_event_buffer* buf,
                          unsigned ts)
{
	struct input_event events[BUFZ];

	size_t len = input_event_buffer_popwhile(buf,
	                                         events,
	                                         BUFZ,
	                                         &ts,
	                                         event_is_before);
	struct input_context* cstate;
	void* current;
	void* state;
	for (size_t i = 0; i < len; ++i) {
		state = NULL;
		while(pllist_movenext(kernel->contexts, &state, &current)) {
			cstate = current;
			if (cstate != NULL && cstate->active
			    && input_state_apply_event(&cstate->state,
			                               events + i))
				break;
		}
	}
}
