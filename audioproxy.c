#include "audio.h"

static struct audio_data* data;
static float proxy_volume = 1.0;

void proxy_init(void* d)
{
	data = d;
}

void proxy_setvolume(float f)
{
	proxy_volume = f;
}

float proxy_getvolume()
{
	return proxy_volume;
}

void play_paddle_hit()
{
	audio_play(data, WAVE_TRIANGLE, 250, 220, 0.75f * proxy_volume);
}

void play_player_point()
{
	audio_play(data, WAVE_SAWTOOTH, 500, 440, 0.75f * proxy_volume);
	audio_play(data, WAVE_SQUARE, 500, 110, 0.75f * proxy_volume);
}

void play_menu_move()
{
	audio_play(data, WAVE_SAWTOOTH, 120, 220, 0.25f * proxy_volume);
}

void play_menu_back()
{
	audio_play(data, WAVE_SAWTOOTH, 500, 220, 0.25f * proxy_volume);
	audio_play(data, WAVE_SQUARE, 250, 330, 0.5f * proxy_volume);
}

void play_menu_confirm()
{
	audio_play(data, WAVE_SAWTOOTH, 500, 220, 0.25f * proxy_volume);
	audio_play(data, WAVE_SQUARE, 250, 110, 0.5f * proxy_volume);
}
