#include "audio.h"

static struct audio_data* data;

void proxy_init(void* d) {
	data = d;
}

void play_paddle_hit()
{
	audio_play(data, WAVE_TRIANGLE, 250, 220, 0.01f);
}

void play_player_point()
{
	audio_play(data, WAVE_SAWTOOTH, 500, 440, 0.01f);
	audio_play(data, WAVE_SQUARE, 500, 110, 0.01f);
}
