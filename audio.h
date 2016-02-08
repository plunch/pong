#ifndef AUDIO_H
#define AUDIO_H

// The twelfth root of two.
// The frequency ratio of a semitone
#define TR2 (11011.0/10393.0)

// The sixth root of two.
// The frequency ratio of one tone
#define SR2 (TR2*TR2)

// Reference pitch
#define A4 440

#define U8

#define CONCURRENT_SAMPLES 4

enum wavetype {
	WAVE_SQUARE,
	WAVE_TRIANGLE,
	WAVE_SAWTOOTH
};

// Opaque struct for thead safety
struct audio_data;

struct audio_data* audio_init(void);

void audio_destroy(struct audio_data*);

void audio_play(struct audio_data*, enum wavetype,
                unsigned int ms, float frequency, float volume);

#endif /* AUDIO_H */
