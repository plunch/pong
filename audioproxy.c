#include "audio.h"

static struct audio_data* data;

void proxy_init(void* d) {
	data = d;
}

void play_paddle_hit()
{
	audio_play(data, WAVE_TRIANGLE, 250, 220, 0.75f);
}

void play_player_point()
{
	audio_play(data, WAVE_SAWTOOTH, 500, 440, 0.75f);
	audio_play(data, WAVE_SQUARE, 500, 110, 0.75f);
}

void play_menu_move()
{
	audio_play(data, WAVE_SAWTOOTH, 120, 220, 0.25f);
}

void play_menu_back()
{
	audio_play(data, WAVE_SAWTOOTH, 500, 220, 0.25f);
	audio_play(data, WAVE_SQUARE, 250, 330, 0.5f);
}

void play_menu_confirm()
{
	audio_play(data, WAVE_SAWTOOTH, 500, 220, 0.25f);
	audio_play(data, WAVE_SQUARE, 250, 110, 0.5f);
}
