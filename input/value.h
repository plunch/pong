#ifndef INPUT_VALUE_H
#define INPUT_VALUE_H

#include "typedefs.h"

enum input_value_type {
	IET_NONE,

	IET_KEY,      // true/false
	IET_AXIS,     // 0-1
	IET_RELATIVE, // Per-frame relative (eg mousex)

	IET_MAX,
};

enum input_key {
	KEY_STATE_DOWN = 0x1,
	KEY_STATE_UP = 0x0,

	KEY_RISING_EDGE = 0x2,
	KEY_FALLING_EDGE = 0x4,

	KEY_STATE_MASK = 0x1,
	KEY_EDGE_MASK = 0x6,

	KEY_PRESSED = 0x3,
	KEY_RELEASED = 0x4,

	KEY_DEFAULT = 0x0,
	KEY_DOWN = 0x1,
};

struct input_value {
	enum input_value_type type;
	union {
		real axis;
		enum input_key key;
		int relative;
	} as;
};

struct input_source_id {
	unsigned int source;
	unsigned int which;
	unsigned int device;
};

static int input_source_id_equals(struct input_source_id* a,
                                  struct input_source_id* b)
{
	return a->source == b->source 
	     && a->which == b->which
	    && a->device == b->device;
}

real input_value_as_axis(const struct input_value*,
                         const struct input_value*);
enum input_key input_value_as_key(const struct input_value*,
                                  const struct input_value*);
int input_value_as_relative(const struct input_value*,
                            const struct input_value*);

#endif /* INPUT_VALUE_H */
