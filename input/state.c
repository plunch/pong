#include "state.h"

#include "value.h"

#include <stdlib.h>
#include <assert.h>

#if 0
#include "value_dbg.h"
#endif

int input_state_create(struct input_state* state, unsigned intents)
{
	assert(state != NULL);
	state->max_intent = intents;
	state->values = malloc(sizeof(struct input_intent_value) * intents);
	if (state->values == NULL)
		return 0;
	state->mappings = NULL;
	state->num_mappings = 0;
	for (unsigned i = 0; i < state->max_intent; ++i) {
		state->values[i].axis = 0;
		state->values[i].key = 0;
		state->values[i].relative = 0;
	}
	return 1;
}

int input_state_resize(struct input_state* state, unsigned intents)
{
	assert(state != NULL);
	if (state->max_intent >= intents) return 1;

	void* p = realloc(state->values,
	                  sizeof(struct input_intent_value) * intents);
	if (p == NULL)
		return 0;
	state->values = p;
	for (unsigned i = state->max_intent; i < intents; ++i) {
		state->values[i].axis = 0;
		state->values[i].key = 0;
		state->values[i].relative = 0;
	}
	state->max_intent = intents;
	input_state_reset(state);
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
	m->last_value = e->value;

#if 0
	printf("Updated %u to ", m->intent);
	print_input_key(target->key);
	printf("\n");
#endif
}

int input_state_apply_event(struct input_state* state, struct input_event* e)
{
	assert(state != NULL);
	assert(e != NULL);

	for(unsigned i = 0; i < state->num_mappings; ++i) {
		if (input_source_id_equals(&e->id,
		                           &state->mappings[i].source)) {
#if 0
			printf("Applied a ");
			print_input_value(e->value);
			printf(" from ");
			print_input_source_id(e->id);
			printf(" to %u\n", (state->mappings + i)->intent);
#endif
			apply_event(state, e, state->mappings + i);
			return 1;
		}
	}
	return 0;
}

int input_state_add_mapping(struct input_state* state,
                            struct input_source_id id,
                            unsigned intent)
{
	assert(state != NULL);

	void* ptr = realloc(state->mappings,
	                    sizeof(struct input_mapping)
	                      * (state->num_mappings + 1));
	if (ptr == NULL)
		return 0;
	state->mappings = ptr;
	unsigned i = state->num_mappings++;
	state->mappings[i].source = id;
	state->mappings[i].intent = intent;
	state->mappings[i].last_value.type = IET_NONE;
	return 1;
}

void input_state_step(struct input_state* state)
{
	assert(state != NULL);

	for (unsigned i = 0; i < state->max_intent; ++i)
		state->values[i].key = state->values[i].key & KEY_STATE_MASK;
}
