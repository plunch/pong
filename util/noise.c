#include "noise.h"

#include <math.h>
#include <assert.h>

#ifndef M_PI
#define M_PI (3.141592653589793)
#endif


static float wave(float frequency, float pos, float phase)
{
	return sinf(M_PI*2 * frequency * pos + phase);
}

float noise_sample(struct noise* n, float position)
{
	assert(n);

	float sum = 0;
	for (unsigned f = 0; f < n->num_frequencies; ++f) {
		float freq      = n->frequencies[f];
		float amplitude = n->amplitudes[f];

		sum += amplitude * wave(freq, position, n->phases[f]);
	}
	return sum / n->amplitude_sum;
}


int noise_init(struct noise* n,
               float         frequency_start,
               float         frequency_end,
               float         exponent,
               void*         ud,
               void*        (*allocate)(void*, size_t))
{
	assert(n != NULL);
	assert(allocate != NULL);

	float* freq = allocate(ud, sizeof(float) * n->num_frequencies * 3);
	float* phas = freq + n->num_frequencies;
	float* amp  = freq + n->num_frequencies * 2;

	if (freq == NULL)
		return 0;

	float freq_step = (frequency_end - frequency_start)
	                  / n->num_frequencies;
	float amp_sum = 0;
	for (unsigned i = 0; i < n->num_frequencies; ++i) {
		freq[i] = frequency_start + i * freq_step;
		phas[i] = rand() / (float)RAND_MAX * M_PI*2;
		amp[i]  = powf(freq[i], exponent);
		amp_sum += amp[i];
	}

	n->frequencies = freq;
	n->phases      = phas;
	n->amplitudes  = amp;
	n->amplitude_sum = amp_sum;
	return 1;
}

void noise_release(struct noise* n,
                   void*         ud,
                   void        (*release)(void*, void*))
{
	assert(n != NULL);
	assert(release != NULL);

	release(ud, n->frequencies);
	n->frequencies = NULL;
	n->phases = NULL;
	n->amplitudes = NULL;
}


static void* allocfunc(void* ud, size_t sz)
{
	(void)(ud);
	return malloc(sz);
}

static void freefunc(void* ud, void* ptr)
{
	(void)(ud);
	free(ptr);
}


int noise_create(struct noise* n,
                 float         fs,
                 float         fe,
                 float         e)
{
	return noise_init(n, fs, fe, e, NULL, allocfunc);
}

void noise_destroy(struct noise* noise)
{
	noise_release(noise, NULL, freefunc);
}


