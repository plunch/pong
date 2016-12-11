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
	int (*write_id)(struct input_source_id, char*, size_t);
	struct input_source_id (*read_id)(const char*, size_t);
};

void input_kernel_consume(struct input_kernel*,
                          struct input_event_buffer*,
                          unsigned);

#endif /* INPUT_KERNEL_H */
