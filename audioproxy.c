#include "audio.h"
#include "audioproxy.h"

#include <stdlib.h>

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
	if (!data) return;
	int x = rand() % 110 - 55;
	audio_play(data, WAVE_TRIANGLE, 250, 220+x, 0.75f * proxy_volume);
}

void play_wall_hit()
{
	if (!data) return;
	int x = rand() % 110 -55;
	audio_play(data, WAVE_NOISE, 250, 440+x, .27f * proxy_volume);
}

void play_player_point()
{
	if (!data) return;
	int x = rand() % 110 - 55;
	audio_play(data, WAVE_SAWTOOTH, 500, 440+x, 0.75f * proxy_volume);
	audio_play(data, WAVE_SQUARE, 500, 110+x, 0.75f * proxy_volume);
}

void play_menu_move()
{
	if (!data) return;
	int x = rand() % 110 - 55;
	audio_play(data, WAVE_SAWTOOTH, 120, 220+x, 0.25f * proxy_volume);
}

void play_menu_back()
{
	if (!data) return;
	int x = rand() % 110 - 55;
	audio_play(data, WAVE_SAWTOOTH, 500, 220+x, 0.25f * proxy_volume);
	audio_play(data, WAVE_SQUARE, 250, 330+x, 0.5f * proxy_volume);
}

void play_menu_confirm()
{
	if (!data) return;
	int x = rand() % 110 - 55;
	audio_play(data, WAVE_SAWTOOTH, 500, 220+x, 0.25f * proxy_volume);
	audio_play(data, WAVE_SQUARE, 250, 110+x, 0.5f * proxy_volume);
}
