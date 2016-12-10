#ifndef INPUT_VALUE_DBG_H
#define INPUT_VALUE_DBG_H

#include <stdio.h>

#include "value.h"

void print_input_source_id(struct input_source_id id)
{
	printf("{source: %u which: %u device %u}",
	       id.source,
	       id.which,
	       id.device);
}

void print_input_key(enum input_key key)
{
	char str[3];
	str[2] = '\0';
	if (key & KEY_EDGE_MASK) {
		if (key & KEY_RISING_EDGE)
			str[0] = 'V';
		else
			str[0] = '^';
	} else {
		str[0] = ' ';
	}
	if (key & KEY_DOWN)
		str[1] = 'd';
	else
		str[1] = 'u';
	printf("{key: %s}", str);
}

void print_input_value(struct input_value v)
{
	switch(v.type) {
		case IET_KEY:
			print_input_key(v.as.key);
			break;
		case IET_AXIS:
			printf("{axis: " PRIREAL "}", v.as.axis);
			break;
		case IET_RELATIVE:
			printf("{rel: %i}", v.as.relative);
			break;
		default:
			printf("?UNKNOWN?");
			break;
	}
}

#endif /* INPUT_VALUE_DBG_H */
