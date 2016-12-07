#include "input/event.h"
#include "input/event_buffer.h"

#include <SDL2/SDL.h>

#define SRC_KEYBOARD 0
#define SRC_MOUSE 1
#define SRC_JBALL 2
#define SRC_JAXIS 3
#define SRC_JBUTTON 4
#define SRC_JHAT 5

#define WHC_MOUSE_WHEEL_X 0
#define WHC_MOUSE_WHEEL_Y 1
#define WHC_MOUSE_X 2
#define WHC_MOUSE_Y 3
#define WHC_MOUSE_BL 4
#define WHC_MOUSE_BR 5
#define WHC_MOUSE_BM 6
#define WHC_MOUSE_B1 7
#define WHC_MOUSE_B2 8



static real rescale_axis(Sint16 value)
{
	if (value < 0)
		return value/(real)32768;
	else
		return value/(real)32767;
}

static unsigned joystickid_to_device(SDL_JoystickID id)
{
	return (unsigned)id;
}


static int mwheel(struct input_event* events, SDL_MouseWheelEvent* e)
{
	events[0].id.source = SRC_MOUSE;
	events[0].id.which = WHC_MOUSE_WHEEL_X;
	events[0].id.device = e->which;
	events[0].value.type = IET_RELATIVE;
	events[0].value.as.relative = e->x;

	events[1].id.source = SRC_MOUSE;
	events[1].id.which = WHC_MOUSE_WHEEL_Y;
	events[1].id.device = e->which;
	events[1].value.type = IET_RELATIVE;
	events[1].value.as.relative = e->y;
	return 2;
}

static int mbutton(struct input_event* events, SDL_MouseButtonEvent* e)
{
	int c = 0;

	if (e->button & SDL_BUTTON_LEFT) {
		events[c].id.source = SRC_MOUSE;
		events[c].id.which = WHC_MOUSE_BL;
		events[c].id.device = e->which;
		events[c].value.type = IET_KEY;
		if (e->state == SDL_PRESSED)
			events[c].value.as.key = KEY_PRESSED;
		else
			events[c].value.as.key = KEY_RELEASED;
		c++;
	}

	if (e->button & SDL_BUTTON_RIGHT) {
		events[c].id.source = SRC_MOUSE;
		events[c].id.which = WHC_MOUSE_BR;
		events[c].id.device = e->which;
		events[c].value.type = IET_KEY;
		if (e->state == SDL_PRESSED)
			events[c].value.as.key = KEY_PRESSED;
		else
			events[c].value.as.key = KEY_RELEASED;
		c++;
	}
	
	if (e->button & SDL_BUTTON_MIDDLE) {
		events[c].id.source = SRC_MOUSE;
		events[c].id.which = WHC_MOUSE_BM;
		events[c].id.device = e->which;
		events[c].value.type = IET_KEY;
		if (e->state == SDL_PRESSED)
			events[c].value.as.key = KEY_PRESSED;
		else
			events[c].value.as.key = KEY_RELEASED;
		c++;
	}

	if (e->button & SDL_BUTTON_X1) {
		events[c].id.source = SRC_MOUSE;
		events[c].id.which = WHC_MOUSE_B1;
		events[c].id.device = e->which;
		events[c].value.type = IET_KEY;
		if (e->state == SDL_PRESSED)
			events[c].value.as.key = KEY_PRESSED;
		else
			events[c].value.as.key = KEY_RELEASED;
		c++;
	}

	if (e->button & SDL_BUTTON_X2) {
		events[c].id.source = SRC_MOUSE;
		events[c].id.which = WHC_MOUSE_B2;
		events[c].id.device = e->which;
		events[c].value.type = IET_KEY;
		if (e->state == SDL_PRESSED)
			events[c].value.as.key = KEY_PRESSED;
		else
			events[c].value.as.key = KEY_RELEASED;
		c++;
	}

	return c;
}

static int mmotion(struct input_event* events, SDL_MouseMotionEvent* e)
{
	events[0].id.source = SRC_MOUSE;
	events[0].id.which = WHC_MOUSE_X;
	events[0].id.device = e->which;
	events[0].value.type = IET_RELATIVE;
	events[0].value.as.relative = e->xrel;

	events[1].id.source = SRC_MOUSE;
	events[1].id.which = WHC_MOUSE_Y;
	events[1].id.device = e->which;
	events[1].value.type = IET_RELATIVE;
	events[1].value.as.relative = e->yrel;
	return 2;
}

static int key(struct input_event* events, SDL_KeyboardEvent* e)
{
	events[0].id.source = SRC_KEYBOARD;
	events[0].id.which = e->keysym.scancode;
	events[0].id.device = 0;
	events[0].value.type = IET_KEY;
	if (e->state == SDL_PRESSED) {
		events[0].value.as.key = KEY_PRESSED;
		if (e->repeat)
			events[0].value.as.key = KEY_STATE_DOWN;
	} else {
		events[0].value.as.key = KEY_RELEASED;
	}
	return 1;
}

static int ball(struct input_event* events, SDL_JoyBallEvent* e)
{
	events[0].id.source = SRC_JBALL;
	events[0].id.which = e->ball;
	events[0].id.device = joystickid_to_device(e->which);
	events[0].value.type = IET_RELATIVE;
	events[0].value.as.relative = e->xrel;

	events[1].id.source = SRC_JBALL;
	events[1].id.which = (unsigned)(e->ball)<<8;
	events[1].id.device = joystickid_to_device(e->which);
	events[1].value.type = IET_RELATIVE;
	events[1].value.as.relative = e->yrel;
	return 2;
}

static int axis(struct input_event* events, SDL_JoyAxisEvent* e)
{
	events[0].id.source = SRC_JAXIS;
	events[0].id.which = e->axis;
	events[0].id.device = joystickid_to_device(e->which);
	events[0].value.type = IET_AXIS;
	events[0].value.as.axis = rescale_axis(e->value);
	return 1;
}

static int button(struct input_event* events, SDL_JoyButtonEvent* e)
{
	events[0].id.source = SRC_JBUTTON;
	events[0].id.which = e->button;
	events[0].id.device = joystickid_to_device(e->which);
	events[0].value.type = IET_KEY;
	if (e->state == SDL_PRESSED)
		events[0].value.as.key = KEY_PRESSED;
	else
		events[0].value.as.key = KEY_RELEASED;
	return 1;
}

static int hat(struct input_event* events, SDL_JoyHatEvent* e)
{
	(void)(events);
	(void)(e);
	return 0;
}

void input_sdl_event(struct input_event_buffer* buffer, SDL_Event* e)
{
	struct input_event events[5];
	int count = 0;

	switch(e->type) {
		case SDL_MOUSEWHEEL:
			count = mwheel(events, &e->wheel);
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			count = mbutton(events, &e->button);
			break;
		case SDL_MOUSEMOTION:
			count = mmotion(events, &e->motion);
			break;


		case SDL_KEYUP:
		case SDL_KEYDOWN:
			count = key(events, &e->key);
			break;

		case SDL_JOYBALLMOTION:
			count = ball(events, &e->jball);
			break;
		case SDL_JOYAXISMOTION:
			count = axis(events, &e->jaxis);
			break;

		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN:
			count = button(events, &e->jbutton);
			break;

		case SDL_JOYHATMOTION:
			count = hat(events, &e->jhat);
			break;
	}

	for(int i = 0; i < count; ++i) {
		events[i].timestamp = e->common.timestamp;
		input_event_buffer_push(buffer, events + i);
	}
}
