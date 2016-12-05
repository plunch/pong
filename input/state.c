#include "state.h"

#include "value.h"

#include <stdlib.h>
#include <assert.h>

int input_state_create(struct input_state* state, unsigned intents)
{
	assert(state != NULL);
	state->max_intent = intents;
	state->values = malloc(sizeof(struct input_value) * intents);
	if (state->values == NULL)
		return 0;
	state->mappings = NULL;
	state->num_mappings = 0;
	return 1;
}

void input_state_release(struct input_state* state)
{
	assert(state != NULL);

	if (state->values != NULL) free(state->values);
	if (state->mappings != NULL) free(state->mappings);
}

void input_state_reset(struct input_state* state)
{
	assert(state != NULL);
	for(unsigned i = 0; i < state->num_mappings; ++i) {
		state->mappings[i].last_value.type = IET_NONE;
	}
}

void input_value_update(struct input_intent_value* target,
                         const struct input_value* current,
                         const struct input_value* old)
{
	target->key = input_value_as_key(current, old);
	target->relative = input_value_as_relative(current, old);
	target->axis = input_value_as_axis(current, old);
}


static void apply_event(struct input_state* state,
                        struct input_event* e,
                      struct input_mapping* m)
{
	struct input_intent_value* target = state->values + m->intent;

	input_value_update(target, &e->value, &m->last_value);
}

int input_state_apply_event(struct input_state* state, struct input_event* e)
{
	assert(state != NULL);
	assert(e != NULL);

	for(unsigned i = 0; i < state->num_mappings; ++i) {
		if (input_source_id_equals(&e->id, &state->mappings[i].source)) {
			apply_event(state, e, state->mappings + i);
			return 1;
		}
	}
	return 0;
}
