#ifndef INPUT_MAPPING_H
#define INPUT_MAPPING_H

#include "value.h"


struct input_mapping {
	struct input_source_id source;
	unsigned intent;
	struct input_value last_value;
};

#endif /* INPUT_MAPPING_H */
