#ifndef INPUT_STATE_H
#define INPUT_STATE_H

#include "value.h"
#include "event.h"
#include "mapping.h"

struct input_intent_value {
	real axis;
	enum input_key key;
	int relative;
};

struct input_state {
	unsigned max_intent;
	struct input_intent_value* values;
	unsigned num_mappings;
	struct input_mapping* mappings;
};

int input_state_create(struct input_state*, unsigned intents);
int input_state_resize(struct input_state*, unsigned intents);
int input_state_add_mapping(struct input_state*,
                            struct input_source_id,
                            unsigned);
int input_state_remove_mapping(struct input_state*, 
                               struct input_source_id,
                               unsigned);
int input_state_remove_mappings(struct input_state*, unsigned);

void input_state_release(struct input_state*);


void input_state_reset(struct input_state*);
void input_state_step(struct input_state*);
int input_state_apply_event(struct input_state*, struct input_event*);

#endif /* INPUT_STATE_H */
