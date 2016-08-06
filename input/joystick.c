#include "joystick.h"
#include <assert.h>
#include <stdlib.h> /* abs */
#include <stdio.h>

static void ball(struct joystick_mapping* m, struct input* in, SDL_JoyBallEvent* e)
{
	for(size_t i = 0; i < m->balen; ++i) {
		if (m->balls[i].joystick == e->which
		 && m->balls[i].ball == e->ball) {
			real input;
			if (m->balls[i].horizontal)
				input = (real)e->xrel;
			else
				input = (real)e->yrel;

			in->input[m->balls[i].action] = input;
		}
	}
}

static void axis(struct joystick_mapping* m, struct input* in, SDL_JoyAxisEvent* e)
{
	fprintf(stderr, "JS: axis=%i value=" PRIREAL ", j=%i\n", (unsigned char)e->axis, (real)e->value, (int)e->which);
	for(size_t i = 0; i < m->alen; ++i) {
		if (m->axes[i].joystick == e->which
		 && m->axes[i].axis == e->axis) {
			real input = 0;
			if (m->axes[i].deadzone < abs(e->value))
				input = (real)e->value;
			in->input[m->axes[i].action] = input;
		}
	}
}

static void button(struct joystick_mapping* m, struct input* in, SDL_JoyButtonEvent* e)
{
	//fprintf(stderr, "JS: button=%i value=%i, j=%i\n", (unsigned char)e->button, e->state == SDL_PRESSED ? 1 : 0, (int)e->which);
	for(size_t i = 0; i < m->blen; ++i) {
		if (m->buttons[i].joystick == e->which
		 && m->buttons[i].button == e->button) {
			real input = e->state == SDL_PRESSED ? 1 : 0;
			in->input[m->buttons[i].action] = input;
		}
	}
}

static void hat(struct joystick_mapping* m, struct input* in, SDL_JoyHatEvent* e)
{
	//fprintf(stderr, "JS: hat=%i value=%i, j=%i\n", (unsigned char)e->hat, e->value, (int)e->which);
	for(size_t i = 0; i < m->hlen; ++i) {
		if (m->hats[i].joystick == e->which
		 && m->hats[i].hat == e->hat) {
			real input = 0;
			if (m->hats[i].ppos == e->value)
				input = 1;
			else if (m->hats[i].npos == e->value)
				input = -1;
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
