#include "sdl.h"
#include "input/event.h"
#include "input/event_buffer.h"

#include <limits.h> /* INT_MAX */
#include <stdio.h>  /* snprintf, sscanf */
#include <string.h> /* strlen, strcpy */
#include <SDL2/SDL.h>

#define SRC_NONE UINT_MAX
#define SRC_KEYBOARD_S 0
#define SRC_KEYBOARD_K 1
#define SRC_MOUSE 2
#define SRC_JBALL 3
#define SRC_JAXIS 4
#define SRC_JBUTTON 5
#define SRC_JHAT 6

#define WHC_MOUSE_WHEEL_X 0
#define WHC_MOUSE_WHEEL_Y 1
#define WHC_MOUSE_X 2
#define WHC_MOUSE_Y 3
#define WHC_MOUSE_BL 4
#define WHC_MOUSE_BR 5
#define WHC_MOUSE_BM 6
#define WHC_MOUSE_B1 7
#define WHC_MOUSE_B2 8

struct jsinfo {
	SDL_Joystick* js;
	SDL_JoystickID id;
	SDL_JoystickGUID guid;
	char* name;
};

static char* strdup(const char* s)
{
	size_t len = strlen(s);
	char* r = malloc(len+1);
	if (r == NULL)
		return r;
	return strncpy(r, s, len);
}

static int jsguid_eq(SDL_JoystickGUID a, SDL_JoystickGUID b)
{
	return memcmp(a.data, b.data, 16);
}

size_t joysticks_len = 0;
struct jsinfo* joysticks = NULL;


struct input_source_id input_sdl_scancode(SDL_Scancode code)
{
	struct input_source_id ret = { SRC_KEYBOARD_S, (unsigned)code, 0 };
	return ret;
}

struct input_source_id input_sdl_keycode(SDL_Keycode code)
{
	struct input_source_id ret = { SRC_KEYBOARD_K, (unsigned)code, 0 };
	return ret;
}


static real rescale_axis(Sint16 value)
{
	if (value < 0)
		return value/(real)32768;
	else
		return value/(real)32767;
}

static void* joystick_addu(SDL_JoystickGUID guid, char* name)
{
	for(size_t i = 0; i < joysticks_len; ++i) {
		if (jsguid_eq(joysticks[i].guid, guid))
			return joysticks + i;
	}

	void* ptr = realloc(joysticks, sizeof(struct jsinfo)
	                        * (joysticks_len+1));
	if (ptr == NULL)
		return NULL;
	joysticks = ptr;
	joysticks[joysticks_len].js = NULL;
	joysticks[joysticks_len].id = INT_MAX;
	joysticks[joysticks_len].guid = guid;
	if (name == NULL)
		name = strdup("");
	joysticks[joysticks_len].name = name;
	return joysticks + joysticks_len++;
}

static void* joystick_add(int sdl_index)
{
	SDL_Joystick* js = SDL_JoystickOpen(sdl_index);
	if (js == NULL)
		return NULL;

	SDL_JoystickGUID guid = SDL_JoystickGetGUID(js);
	SDL_JoystickID id = SDL_JoystickInstanceID(js);

	for(size_t i = 0; i < joysticks_len; ++i) {
		if (jsguid_eq(joysticks[i].guid, guid)) {
			joysticks[i].js = js;
			joysticks[i].id = id;

			const char* n = SDL_JoystickName(js);
			if (n != NULL) {
				if (joysticks[i].name == NULL)
					free(joysticks[i].name);
				joysticks[i].name = strdup(n);
			}
			return joysticks + i;
		}
	}

	void* ptr = realloc(joysticks, sizeof(struct jsinfo)
	                                * (joysticks_len+1));
	if (ptr == NULL)
		return NULL;
	joysticks = ptr;
	joysticks[joysticks_len].js = js;
	joysticks[joysticks_len].id = id;
	joysticks[joysticks_len].guid = guid;
	const char* n = SDL_JoystickName(js);
	if (n == NULL)
		n = "";
	joysticks[joysticks_len].name = strdup(n);
	return joysticks + joysticks_len++;
}

static void joystick_remove(SDL_JoystickID id)
{
	SDL_Joystick* js = SDL_JoystickFromInstanceID(id);
	for (size_t i = 0; i < joysticks_len; ++i) {
		if (joysticks[i].js == js) {
			SDL_JoystickClose(js);
			joysticks[i].js = NULL;
		}
	}
}

static void* joystickid_to_device(SDL_JoystickID id)
{
	for(size_t i = 0; i < joysticks_len; ++i) {
		if (joysticks[i].id == id)
			return joysticks + i;
	}
	return NULL;
}

static SDL_JoystickID device_to_joystickid(void* device)
{
	struct jsinfo* info = device;
	return info->id;
}

static void* mouseid_to_device(unsigned mouseid)
{
	return (void*)mouseid;
}

static unsigned device_to_mouseid(void* device)
{
	return (unsigned)device;
}


static int mwheel(struct input_event* events, SDL_MouseWheelEvent* e)
{
	events[0].id.source = SRC_MOUSE;
	events[0].id.which = WHC_MOUSE_WHEEL_X;
	events[0].id.device = mouseid_to_device(e->which);
	events[0].value.type = IET_RELATIVE;
	events[0].value.as.relative = e->x;

	events[1].id.source = SRC_MOUSE;
	events[1].id.which = WHC_MOUSE_WHEEL_Y;
	events[1].id.device = mouseid_to_device(e->which);
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
		events[c].id.device = mouseid_to_device(e->which);
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
		events[c].id.device = mouseid_to_device(e->which);
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
		events[c].id.device = mouseid_to_device(e->which);
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
		events[c].id.device = mouseid_to_device(e->which);
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
		events[c].id.device = mouseid_to_device(e->which);
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
	events[0].id.device = mouseid_to_device(e->which);
	events[0].value.type = IET_RELATIVE;
	events[0].value.as.relative = e->xrel;

	events[1].id.source = SRC_MOUSE;
	events[1].id.which = WHC_MOUSE_Y;
	events[1].id.device = mouseid_to_device(e->which);
	events[1].value.type = IET_RELATIVE;
	events[1].value.as.relative = e->yrel;
	return 2;
}

static int key(struct input_event* events, SDL_KeyboardEvent* e)
{
	events[0].id.source = SRC_KEYBOARD_S;
	events[0].id.which = e->keysym.scancode;
	events[0].id.device = NULL;
	events[0].value.type = IET_KEY;
	if (e->state == SDL_PRESSED) {
		events[0].value.as.key = KEY_PRESSED;
		if (e->repeat)
			events[0].value.as.key = KEY_STATE_DOWN;
	} else {
		events[0].value.as.key = KEY_RELEASED;
	}

	events[1].id.source = SRC_KEYBOARD_K;
	events[1].id.which = (unsigned)e->keysym.sym;
	events[1].id.device = NULL;
	events[1].value.type = IET_KEY;
	if (e->state == SDL_PRESSED) {
		events[1].value.as.key = KEY_PRESSED;
		if (e->repeat)
			events[1].value.as.key = KEY_STATE_DOWN;
	} else {
		events[1].value.as.key = KEY_RELEASED;
	}
	return 2;
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

		case SDL_JOYDEVICEADDED:
			joystick_add(e->jdevice.which);
			break;
		case SDL_JOYDEVICEREMOVED:
			joystick_remove(e->jdevice.which);
			break;
	}

	for(int i = 0; i < count; ++i) {
		events[i].timestamp = e->common.timestamp;
		input_event_buffer_push(buffer, events + i);
	}
}







static char get_src_char(unsigned src)
{
	switch(src) {
		case SRC_KEYBOARD_S:
			return 's';
		case SRC_KEYBOARD_K:
			return 'k';
		case SRC_MOUSE:
			return 'm';
		case SRC_JBALL:
			return 'l';
		case SRC_JAXIS:
			return 'a';
		case SRC_JBUTTON:
			return 'b';
		case SRC_JHAT:
			return 'h';
		default:
			return '0';
	}
}

static int write_js(struct input_source_id id, char* target, size_t max)
{
	struct jsinfo* js = id.device;
	if (js == NULL) {
		if (max > 0)
			target[0] = '\0';
		return 0;
	}
	int written = snprintf(target,
	                       max,
	                       "%c: %u/",
	                       get_src_char(id.source),
	                       id.which);
	if (written < 0 || (size_t)written > max)
		return written;
	SDL_JoystickGetGUIDString(js->guid,
	                          target + written,
	                          max > INT_MAX ? INT_MAX : (int)max);
	written += 32;
	if ((size_t)written > max) return written;
	if (js->name != NULL) {
		int wr2 = snprintf(target,
		                   max - (size_t)written,
		                   ";%zu;%s",
		                   strlen(js->name),
		                   js->name);
		written += wr2;
	}
	return written;
}

static int write_kb(struct input_source_id id, char* target, size_t max)
{
	return snprintf(target, max, "%c: %u", get_src_char(id.source), id.which);
}

static int write_mouse(struct input_source_id id, char* target, size_t max)
{
	return snprintf(target,
	                max, 
	                "%c: %u/%u",
	                get_src_char(id.source),
	                id.which,
	                device_to_mouseid(id.device));
}

int input_sdl_write_id(struct input_source_id id, char* target, size_t max)
{
	switch(id.source) {
		case SRC_KEYBOARD_S:
		case SRC_KEYBOARD_K:
			return write_kb(id, target, max);
		case SRC_MOUSE:
			return write_mouse(id, target, max);
		case SRC_JBALL:
		case SRC_JAXIS:
		case SRC_JBUTTON:
		case SRC_JHAT:
			return write_js(id, target, max);
		default:
			if (max > 0)
				target[0] = '\0';
			return 0;
	}

}

static void* read_js_device(const char* src, size_t max)
{
	if (src[0] == '/') {
		SDL_JoystickGUID guid = SDL_JoystickGetGUIDFromString(src+1);
		char* name = NULL;
		size_t start = 0;
		size_t end = 0;
		int hs = 0;
		for(size_t i = 0; i < max; ++i) {
			if (src[i] == ';') {
				if (!hs) {
					start = i+1;
				} else {
					end = i-1;
					break;
				}
			}
		}
		if (end > start) {
			unsigned long len = strtoul(src + start, NULL, 0);
			name = malloc(len+1);
			if (name != NULL) {
				strncpy(name, src + end + 2, len);
				name[len] = '\0';
			}
		}
		return joystick_addu(guid, name);
	} else {
		return NULL;
	}
}

static void* read_mouse_device(const char* src, size_t max)
{
	(void)(max);
	if (src[0] == '/') {
		unsigned long ul = strtoul(src + 1, NULL, 0);
		return mouseid_to_device(ul > UINT_MAX
		                          ? UINT_MAX
		                          : (unsigned)ul);
	} else {
		return NULL;
	}
}

static void* read_kb_device(const char* src, size_t max)
{
	(void)(max);
	(void)(src);
	return NULL;
}


struct input_source_id input_sdl_read_id(const char* src, size_t max)
{
	struct input_source_id res = {0, 0, 0};

	if (max < 3)
		return res;

	if (src[0] == '0')
		return res;
	if (src[1] != ':')
		return res;

	char* more;
	unsigned long ul = strtoul(src, &more, 0);
	if (more == src || ul > UINT_MAX
	     || src > more || (size_t)(src - more) > max)
		return res;
	res.which = (unsigned)ul;
	max -= (size_t)(src - more);

	switch(src[0]) {
		case 's':
			res.source = SRC_KEYBOARD_S;
			res.device = read_kb_device(more, max);
			return res;
		case 'k':
			res.source = SRC_KEYBOARD_K;
			res.device = read_kb_device(more, max);
			return res;
		case 'm':
			res.source = SRC_MOUSE;
			res.device = read_mouse_device(more, max);
			return res;
		case 'l':
			res.source = SRC_JBALL;
			res.device = read_js_device(more, max);
			return res;
		case 'a':
			res.source = SRC_JAXIS;
			res.device = read_js_device(more, max);
			return res;
		case 'b':
			res.source = SRC_JBUTTON;
			res.device = read_js_device(more, max);
			return res;
		case 'h':
			res.source = SRC_JHAT;
			res.device = read_js_device(more, max);
			return res;
		default:
			res.source = SRC_NONE;
			return res;
	}
}

