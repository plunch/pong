#include "value.h"

#include <assert.h>
#include <stdlib.h>
#include <math.h>

#define DEFAULT_AXIS 0
#define DEFAULT_RELATIVE 0
#define DEFAULT_KEY KEY_DEFAULT

#define IS_AXIS_POSITIVE(X) ((X) > .25)

#define IS_RELATIVE_POSITIVE(X) (abs(X) > 1)

#define AXIS_2_RELATIVE_SCALE 100
#define KEY_2_RELATIVE 100
#define RELATIVE_2_AXIS_SCALE 0.1


real input_value_as_axis(const struct input_value* value,
                         const struct input_value* old)
{
	assert(value != NULL);
	(void)(old);

	switch(value->type) {
		case IET_KEY:
			return (value->as.key & KEY_STATE_DOWN) ? 1 : 0;
		case IET_AXIS:
			return value->as.axis;
		case IET_RELATIVE:
			return value->as.relative * RELATIVE_2_AXIS_SCALE;
		default:
			return DEFAULT_AXIS;
	}
}

static enum input_key key2key(enum input_key val, enum input_key old)
{
	// If both are 'just pressed' or 'just released', pretend
	// that never happened
	if ((old & KEY_EDGE_MASK) == (val & KEY_EDGE_MASK))
		return val & KEY_STATE_MASK;
	else
		return val;
}

static enum input_key axis2key(real val, real old)
{
	enum input_key ret;
	if (IS_AXIS_POSITIVE(val)) {
		ret = KEY_STATE_DOWN;
		if (!IS_AXIS_POSITIVE(old))
			return ret | KEY_RISING_EDGE;
		else
			return ret;
	} else {
		ret = KEY_DEFAULT;
		if (IS_AXIS_POSITIVE(old))
			return ret | KEY_FALLING_EDGE;
		else
			return ret;
	}
}

static enum input_key relative2key(int val, int old)
{
	enum input_key ret;
	if (IS_RELATIVE_POSITIVE(val)) {
		ret = KEY_STATE_DOWN;
		if (!IS_RELATIVE_POSITIVE(old))
			return ret | KEY_RISING_EDGE;
		else
			return ret;
	} else {
		ret = KEY_DEFAULT;
		if (IS_RELATIVE_POSITIVE(old))
			return ret | KEY_FALLING_EDGE;
		else
			return ret;
	}
}

enum input_key input_value_as_key(const struct input_value* value,
                                  const struct input_value* old)
{
	assert(value != NULL);

	switch(value->type) {
		case IET_KEY:
			if (old != NULL && value->type == old->type)
				return key2key(value->as.key, old->as.key);
			else
				return value->as.key;
		case IET_AXIS:
			if (old != NULL && value->type == old->type)
				return axis2key(value->as.axis,
				                old->as.axis);
			else
				return IS_AXIS_POSITIVE(value->as.axis) 
				        ? KEY_DOWN
				        : KEY_DEFAULT;
		case IET_RELATIVE:
			if (old != NULL && value->type == old->type)
				return relative2key(value->as.relative,
				                    old->as.relative);
			else
				return IS_RELATIVE_POSITIVE(value->
				                            as.relative) 
				        ? KEY_DOWN
				        : KEY_DEFAULT;
		default:
			return DEFAULT_KEY;
	}
}

int input_value_as_relative(const struct input_value* value,
                            const struct input_value* old)
{
	assert(value != NULL);
	(void)(old);

	switch(value->type) {
		case IET_KEY:
			return (value->as.key & KEY_STATE_DOWN)
			         ? KEY_2_RELATIVE
			         : 0;
		case IET_AXIS:
			return (int)lrint_real(AXIS_2_RELATIVE_SCALE 
			                       * value->as.axis);
		case IET_RELATIVE:
			return value->as.relative;
		default:
			return DEFAULT_RELATIVE;
	}
}


