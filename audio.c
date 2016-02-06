#include "audio.h"
#include "global.h"

#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

struct sample {
	float frequency, volume;
	enum wavetype type;

	size_t remaining;
};

struct audio_data {
	SDL_AudioDeviceID dev;
	SDL_AudioSpec spec;

	size_t sample;

	struct sample samples[CONCURRENT_SAMPLES];
};

static float sample(int audiofreq, struct sample* s, size_t i)
{
	float f = (float)audiofreq/s->frequency;
	float v = fmodf((float)i, f) / f;

	switch(s->type) {
		case WAVE_SQUARE:
			return v < 0.5f ? 1 : 0;
		case WAVE_TRIANGLE:
			return fabsf(v - 0.5f) * 2;
		case WAVE_SAWTOOTH:
			return v;
		default:
			return 0;
	}
}


// SDL will call this to supply buffers for the audio driver
static void sdl_audio_callback(void* userdata, Uint8* stream, int len)
{
	if (len <= 0) return;

	size_t l = (size_t)len;

	struct audio_data* data = userdata;

	for(size_t i = 0; i < l; ++i) {
		int contrib = 0;

		float towrite = 0;

		for(size_t sidx = 0; sidx < CONCURRENT_SAMPLES; ++sidx) {
			if (data->samples[sidx].remaining > i) {
				contrib++;

				towrite += sample(data->spec.freq,
				                  &data->samples[sidx],
				                  data->sample + i);
			}
		}

		if (contrib > 0)
			towrite /= (float)contrib;


		if (towrite > 1) towrite = 1;
		if (towrite < 0) towrite = 0;
		stream[i] = (Uint8)(UINT8_MAX * towrite);
	}

	// Update samples
	for(size_t sidx = 0; sidx < CONCURRENT_SAMPLES; ++sidx)
	{
		if (data->samples[sidx].remaining < l) {
			// All written
			data->samples[sidx].remaining = 0;
		} else {
			data->samples[sidx].remaining -= l;
		}
	}

	data->sample += l;
}

void audio_play(struct audio_data* d, enum wavetype type,
                unsigned int ms, float freq, float vol)
{
	// Don't play if audio is disabled
	if (d->dev <= 0) return;

	SDL_LockAudioDevice(d->dev);

	int idx = -1;
	for(int i = 0; i < CONCURRENT_SAMPLES; ++i) {
		if (d->samples[i].remaining == 0) {
			idx = i;
			break;
		}
	}
	if (idx < 0) {
		// queue full
		SDL_UnlockAudioDevice(d->dev);
	}

	float samples_per_ms = (float)d->spec.freq / 1000.0f;

	d->samples[idx].frequency = freq;
	d->samples[idx].volume = vol;
	d->samples[idx].type = type;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbad-function-cast"
	d->samples[idx].remaining = (size_t)ceilf(samples_per_ms * (float)ms); 
#pragma GCC diagnostic pop

	SDL_UnlockAudioDevice(d->dev);
}



struct audio_data* audio_init()
{
	struct audio_data* data = malloc(sizeof(struct audio_data));
	if (data == NULL)
		return NULL;

	SDL_AudioSpec want;

	want.freq = 48000;
	want.format = AUDIO_U8;
	want.channels = 2;
	want.samples = 2048;
	want.callback = &sdl_audio_callback;
	want.userdata = data;

	data->dev = SDL_OpenAudioDevice(NULL,
	                                0, // We don't want to record
	                                &want,
	                                &data->spec,
	                                SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

	if (data->dev > 0) {
		SDL_PauseAudioDevice(data->dev, 0);
	} else {
		sdl_error("Audio disabled");
	}

	for(int i = 0; i < CONCURRENT_SAMPLES; ++i) {
		data->samples[i].remaining = 0;
	}

	return data;
}

void audio_destroy(struct audio_data* d)
{
	if (d->dev > 0)
		SDL_CloseAudioDevice(d->dev);

	free(d);
}

