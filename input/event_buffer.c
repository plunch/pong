#include "input/event_buffer.h"

#include <assert.h>


int input_event_buffer_init(struct input_event_buffer* buffer)
{
	assert(buffer != NULL);
	buffer->start = 0;
	buffer->size = 0;

	buffer->mut = mutex_create();

	return buffer->mut != NULL;
}

void input_event_buffer_free(struct input_event_buffer* buffer)
{
	assert(buffer != NULL);

	mutex_free(buffer->mut);
}

int input_event_buffer_push(struct input_event_buffer* buffer,
                                   struct input_event* value)
{
	assert(buffer != NULL);
	assert(value != NULL);


	if (!mutex_lock(buffer->mut))
		return 0;

	int result = 0;

	if (buffer->size == INPUT_EVENT_BUFZ)
		goto unlock;

	size_t offset = buffer->size++;
	size_t index = (buffer->start + offset) % INPUT_EVENT_BUFZ;
	buffer->events[index] = *value;
	result = 1;
	
unlock:
	mutex_unlock(buffer->mut);
	return result;
}

int input_event_buffer_pop(struct input_event_buffer* buffer,
                                  struct input_event* output)
{
	assert(buffer != NULL);
	assert(output != NULL);

	if (!mutex_lock(buffer->mut))
		return 0;

	int result = 0;

	if (buffer->size == 0)
		goto unlock;

	size_t index = buffer->start;

	*output = buffer->events[index];

	buffer->start++;
	if (buffer->start == INPUT_EVENT_BUFZ)
		buffer->start = 0;
	buffer->size--;
	result = 1;

unlock:
	mutex_unlock(buffer->mut);
	return result;
}

int input_event_buffer_peek(struct input_event_buffer* buffer,
                                   struct input_event* output)
{
	assert(buffer != NULL);
	assert(output != NULL);

	if (!mutex_lock(buffer->mut))
		return 0;

	int result = 0;

	if (buffer->size == 0)
		goto unlock;

	size_t index = buffer->start;

	*output = buffer->events[index];

	result = 1;

unlock:
	mutex_unlock(buffer->mut);
	return result;
}

int input_event_buffer_popif(struct input_event_buffer* buffer,
                                    struct input_event* output,
                                                  void* userdata,
                                                  int (*predicate)(void*, struct input_event*))
{
	assert(buffer != NULL);
	assert(output != NULL);
	assert(predicate != NULL);

	if (!mutex_lock(buffer->mut))
		return 0;

	int result = 0;

	if (buffer->size == 0)
		goto unlock;

	size_t index = buffer->start;

	if (predicate(userdata, buffer->events + index)) {
		*output = buffer->events[index];

		buffer->start++;
		if (buffer->start == INPUT_EVENT_BUFZ)
			buffer->start = 0;
		buffer->size--;

		result = 1;
	}

unlock:
	mutex_unlock(buffer->mut);
	return result;
}

size_t input_event_buffer_popwhile(struct input_event_buffer* buffer,
                                          struct input_event* output,
                                                       size_t len,
                                                        void* userdata,
                                                        int (*predicate)(void*, struct input_event*))
{
	assert(buffer != NULL);
	assert(predicate != NULL);
	assert(len == 0 || output != NULL);

	if (len == 0)
		return 0;

	if (!mutex_lock(buffer->mut))
		return 0;

	size_t result = 0;

	while(len > 0) {
		if (buffer->size == 0)
			break;

		size_t index = buffer->start;

		if (predicate(userdata, buffer->events + index)) {
			*(output + result) = buffer->events[index];

			buffer->start++;
			if (buffer->start == INPUT_EVENT_BUFZ)
				buffer->start = 0;
			buffer->size--;

			result++;
			len--;
		} else {
			break;
		}
	}

	mutex_unlock(buffer->mut);
	return result;
}
