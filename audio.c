#include "audio.h"
#include "global.h"

#include "util/noise.h"
#include "util/interpolation.h"

#include <stdint.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

static float randf()
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
	return rand() / (float)RAND_MAX;
#pragma GCC diagnostic pop
}

struct sample {
	float frequency, volume;
	enum wavetype type;

	size_t remaining, started, ending;

	union {
		struct noise wnoise;
		struct {
			float last;
			float next;
			size_t sample;
		} snoise;
		struct {
			float last;
			size_t sample;
		} noise;
	} p;
};

struct audio_data {
	SDL_AudioDeviceID dev;
	SDL_AudioSpec spec;

	SDL_AudioCVT conversion;

	size_t sample;

	struct sample samples[CONCURRENT_SAMPLES];
};

static void finalize_sample(struct sample* s)
{
	switch(s->type) {
		default: return;

		case WAVE_WNOISE:
			 noise_destroy(&s->p.wnoise);
			 break;
	}
}

static float sample_volume(struct sample* s, size_t i)
{
	(void)(s);
	(void)(i);
	return 1;
	/*
	size_t c = s->ending - s->started;
	i -= s->started;

	return 1 - ((float)i / (float)c);
	*/
}

static float sample(int audiofreq, struct sample* s, size_t i)
{
	float f = (float)audiofreq/s->frequency;
	float v = fmodf((float)i, f) / f;
	v = (v - 0.5f) * 2;


	switch(s->type) {
		case WAVE_SQUARE:
			return v > 0.0f? 1.0f : -1.0f;
		case WAVE_TRIANGLE:
			return fabsf(v)*2-1;
		case WAVE_SAWTOOTH:
			return v;
		case WAVE_WNOISE:
			return noise_sample(&s->p.wnoise, v);
		case WAVE_NOISE:
			if (i <= s->p.noise.sample) {
				s->p.noise.last = randf()*2-1;
				s->p.noise.sample = i + (size_t)f;
			}
			return s->p.noise.last;
		case WAVE_SNOISE:
			;
			if (i >= s->p.snoise.sample) {
				s->p.snoise.last = s->p.snoise.next;
				s->p.snoise.next = randf()*2-1;
				s->p.snoise.sample = i + (size_t)f;
				if (s->p.snoise.next > s->ending) {
					s->p.snoise.next = 0;
					s->p.snoise.sample = s->ending+1;
				}
			}
			float pos = 1-(float)(s->p.snoise.sample - i) / f;
			return icosinef(pos, s->p.snoise.last,
			                     s->p.snoise.next);
		default:
			return .0f;
	}
}


// SDL will call this to supply buffers for the audio driver
static void sdl_audio_callback(void* userdata, Uint8* stream, int len)
{
	if (len <= 0) return;


	struct audio_data* data = userdata;

	size_t l;
	if (data->conversion.needed) {
		double newlen = ceil(len / data->conversion.len_ratio);
		data->conversion.buf = stream;
		data->conversion.len = (int)newlen;
		l = (size_t)newlen;
	} else {
		l = (size_t)len;
	}

	memset(stream, (int)data->spec.silence, (size_t)len);


	for(size_t i = 0; i < l; ++i) {
		float contrib = 0;

		float towrite = 0;

		for(size_t sidx = 0; sidx < CONCURRENT_SAMPLES; ++sidx) {
			if (data->samples[sidx].remaining > i) {
				contrib += data->samples[sidx].volume;

				float vol = data->samples[sidx].volume;
				vol *= sample_volume(&data->samples[sidx],
				                     data->sample + i);

				towrite += sample(data->spec.freq,
				                  &data->samples[sidx],
				                  data->sample + i) * vol;
			}
		}

		if (contrib > 1)
			towrite /= contrib;


		if (towrite > 1) towrite = 1;
		if (towrite < -1) towrite = -1;
		stream[i] = (Uint8)((towrite + 1) * 127);
	}
#ifdef AUDIO_TO_STDOUT
#if AUDIO_TO_STDOUT > 0
	fwrite(stream, sizeof(Uint8), l, stdout);
#endif
#endif
	if (data->conversion.needed)
		SDL_ConvertAudio(&data->conversion);

	// Update samples
	for(size_t sidx = 0; sidx < CONCURRENT_SAMPLES; ++sidx) {
		if (data->samples[sidx].remaining < l) {
			// All written
			data->samples[sidx].remaining = 0;
			finalize_sample(data->samples + sidx);
		} else {
			data->samples[sidx].remaining -= l;
		}
	}

	data->sample += l;
}

static void wave_wnoise_init(struct audio_data* d,
                             struct sample* s,
                             unsigned ms,
                             float freq,
                             float vol)
{
	UNUSED(d);
	UNUSED(ms);
	UNUSED(vol);

	s->p.wnoise.num_frequencies = 14;
	if (!noise_create(&s->p.wnoise, freq, freq*64, WHITE_NOISE_EXP)) {
		s->p.wnoise.num_frequencies = 0;
	}
}

static void wave_noise_init(struct audio_data* d,
                            struct sample* s,
                            unsigned ms,
                            float freq,
                            float vol)
{
	UNUSED(ms);
	UNUSED(vol);

	s->p.noise.last = 0;
	s->p.noise.sample = d->sample + (unsigned)d->spec.freq/(unsigned)freq;
}

static void wave_snoise_init(struct audio_data* d,
                             struct sample* s,
                             unsigned ms,
                             float freq,
                             float vol)
{
	UNUSED(ms);
	UNUSED(vol);

	s->p.noise.sample = d->sample + (unsigned)d->spec.freq/(unsigned)freq;
	s->p.snoise.next = randf()*2-1;
	s->p.snoise.last = 0;
}

void audio_play(struct audio_data* d, enum wavetype type,
                unsigned int ms, float freq, float vol)
{
	// Don't play if audio is disabled
	if (d->dev <= 0) return;

	// WTF
	ms /= 2;

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
		return;
	}

	switch(type) {
		default: break;

		case WAVE_WNOISE:
			 wave_wnoise_init(d, &d->samples[idx], ms, freq, vol);
			 break;

		case WAVE_NOISE:
			 wave_noise_init(d, &d->samples[idx], ms, freq, vol);
			 break;

		case WAVE_SNOISE:
			 wave_snoise_init(d, &d->samples[idx], ms, freq, vol);
			 break;
	}

	float samples_per_ms = (float)d->spec.freq / 1000.0f;

	d->samples[idx].frequency = freq;
	d->samples[idx].volume = vol;
	d->samples[idx].type = type;
	d->samples[idx].remaining = (size_t)(lrintf(samples_per_ms * (float)ms));
	d->samples[idx].started = d->sample;
	d->samples[idx].ending = d->sample + d->samples[idx].remaining;


	SDL_UnlockAudioDevice(d->dev);
}



struct audio_data* audio_init()
{
	struct audio_data* data = malloc(sizeof(struct audio_data));
	if (data == NULL)
		return NULL;
	data->sample = 0;

	SDL_AudioSpec want;

	want.freq = 44100;
	want.format = AUDIO_U8;
	want.channels = 1;
	want.samples = 512;
	want.callback = &sdl_audio_callback;
	want.userdata = data;

	data->dev = SDL_OpenAudioDevice(NULL,
	                                0, // We don't want to record
	                                &want,
	                                &data->spec,
	                                SDL_AUDIO_ALLOW_ANY_CHANGE);

	if (data->dev > 0) {
		SDL_PauseAudioDevice(data->dev, 0);
		SDL_BuildAudioCVT(&data->conversion,
		                  AUDIO_U8, 1, data->spec.samples,
		                  data->spec.format, 1, data->spec.samples);
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

