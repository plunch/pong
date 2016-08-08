#include "joystick.h"
#include <assert.h>
#include <stdlib.h> /* abs */
#include <stdio.h>

static real rescale_axis(Sint16 value)
{
	if (value < 0)
		return value/(real)32768;
	else
		return value/32767;
}

static void ball(struct joystick_mapping* m, struct input* in, SDL_JoyBallEvent* e)
{
#if JOY_DBG & 0x1
	fprintf(stderr, "JS: j%i ball=%i xrel=%i yrel=%i\n",
	                (int)e->which,
	                (unsigned char)e->ball,
	                e->xrel, e->yrel);
#endif
	for(size_t i = 0; i < m->balen; ++i) {
		if (m->balls[i].joystick == e->which
		 && m->balls[i].ball == e->ball) {
			real input;
			// TODO: Can we even translate trackball motion
			// to an 'axis' ?
			if (m->balls[i].horizontal)
				input = rescale_axis(e->xrel);
			else
				input = rescale_axis(e->yrel);

			in->input[m->balls[i].action] = input;
		}
	}
}

static void axis(struct joystick_mapping* m, struct input* in, SDL_JoyAxisEvent* e)
{
#if JOY_DBG & 0x1
	fprintf(stderr, "JS: j%i axis=%i value=%i\n",
	                (int)e->which,
	                (unsigned char)e->axis,
	                e->value);
#endif
	for(size_t i = 0; i < m->alen; ++i) {
		if (m->axes[i].joystick == e->which
		 && m->axes[i].axis == e->axis) {
			real input = 0;
			if (m->axes[i].deadzone < abs(e->value))
				input = rescale_axis(e->value);
			in->input[m->axes[i].action] = input;
		}
	}
}

static void button(struct joystick_mapping* m,
                   struct input* in,
                   SDL_JoyButtonEvent* e)
{
#if JOY_DBG & 0x1
	fprintf(stderr, "JS: j%i button=%i pressed=%s\n",
	                (int)e->which,
	                (unsigned char)e->button,
	                e->state == SDL_PRESSED ? "true" : "false");
#endif
	for(size_t i = 0; i < m->blen; ++i) {
		if (m->buttons[i].joystick == e->which) {
			if (m->buttons[i].pbutton == e->button) {
				if (e->state == SDL_RELEASED && I_P(in, m->buttons[i].action))
					in->input[m->buttons[i].action] = I_ZERO;
				else if (e->state == SDL_PRESSED)
					in->input[m->buttons[i].action] = I_MAX;
			} else if (m->buttons[i].nbutton == e->button) {
				if (e->state == SDL_RELEASED && I_N(in, m->buttons[i].action))
					in->input[m->buttons[i].action] = I_ZERO;
				else if (e->state == SDL_PRESSED)
					in->input[m->buttons[i].action] = I_MIN;
			}
		}
	}
}

static void hat(struct joystick_mapping* m, struct input* in,
               SDL_JoyHatEvent* e)
{
#if JOY_DBG & 0x1
	fprintf(stderr, "JS: j=%i hat=%i direction=%i\n",
	                (int)e->which,
	                (unsigned char)e->hat,
	                (int)e->value);
#endif
	for(size_t i = 0; i < m->hlen; ++i) {
		if (m->hats[i].joystick == e->which
		 && m->hats[i].hat == e->hat) {
			real input = I_ZERO;
			if (m->hats[i].ppos == e->value)
				input = I_MAX;
			else if (m->hats[i].npos == e->value)
				input = I_MIN;
			in->input[m->hats[i].action] = input;
		}
	}
}



static void event(void* data, struct input* in, SDL_Event* e)
{
	struct joystick_mapping* m = data;

	switch(e->type) {
		case SDL_JOYBALLMOTION:
			ball(m, in, &e->jball);
			return;
		case SDL_JOYAXISMOTION:
			axis(m, in, &e->jaxis);
			return;
		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN:
			button(m, in, &e->jbutton);
			return;
		case SDL_JOYHATMOTION:
			hat(m, in, &e->jhat);
			return;
		default:
			return;
	}
}

void jsinput_make_inputsource(struct inputsource_sdl* target, struct joystick_mapping* map)
{
	assert(target != NULL);
	assert(map != NULL);
	target->data = map;
	target->apply = &event;
}
