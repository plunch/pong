#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H

#include "value.h"

struct input_event {
	struct input_source_id id;
	struct input_value value;
};

#endif /* INPUT_EVENT_H */
