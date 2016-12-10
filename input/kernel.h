#ifndef INPUT_KERNEL_H
#define INPUT_KERNEL_H

#include "event_buffer.h"
#include "util/ptrllist.h"
#include "state.h"

struct input_context {
	struct input_state state;
	int active;
	const char* name;
};

struct input_kernel {
	pllist* contexts;
};

void input_kernel_consume(struct input_kernel*,
                          struct input_event_buffer*,
                          unsigned);

#endif /* INPUT_KERNEL_H */
