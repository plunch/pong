#ifndef NOISE_H
#define NOISE_H

#include <stdlib.h>

#define RED_NOISE_EXP   -2
#define PINK_NOISE_EXP  -1
#define WHITE_NOISE_EXP  0
#define BLUE_NOISE_EXP   1
#define VIOLET_NOISE_EXP 2

struct noise {
	float*   frequencies;
	float*   phases;
	float*   amplitudes;
	float    amplitude_sum;
	unsigned num_frequencies;
};

float noise_sample(struct noise* noise, float position);

int noise_init(struct noise* noise,
               float         frequency_start,
               float         frequency_end,
               float         exponent,
	       void*         userdata,
               void*       (*allocate)(void*, size_t));

void noise_release(struct noise* noise,
		   void*         userdata,
		   void        (*release)(void*, void*));

void noise_destroy(struct noise* noise);

int noise_create(struct noise* noise,
                 float         frequency_start,
                 float         frequency_end,
                 float         exponent);


#endif /* NOISE_H */
