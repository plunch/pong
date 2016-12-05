#ifndef INPUT_JOYSTICK_H
#define INPUT_JOYSTICK_H

#include "../input.h"
#include "../inputsource_sdl.h"

#include <SDL2/SDL.h>

struct joyaxis_mapping {
	SDL_JoystickID joystick;
	Uint8 axis;
	Sint16 deadzone;
	unsigned int negative : 1;
	enum game_action action;
};

struct joybutton_mapping {
	SDL_JoystickID joystick;
	Uint8 button;
	enum game_action action;
};

struct joyhat_mapping {
	SDL_JoystickID joystick;
	Uint8 hat;
	Uint8 pos;
	enum game_action action;
};

enum joyball_direction {
	JBD_UP,
	JBD_DOWN,
	JBD_LEFT,
	JBD_RIGHT
};

struct joyball_mapping {
	SDL_JoystickID joystick;
	Uint8 ball;
	enum joyball_direction direction;
	enum game_action action;
};

struct joystick_mapping {
	struct joyaxis_mapping* axes;
	struct joybutton_mapping* buttons;
	struct joyhat_mapping* hats;
	struct joyball_mapping* balls;
	size_t alen, blen, balen, hlen;
};


void jsinput_make_inputsource(struct inputsource_sdl*, struct joystick_mapping*);

#endif /* INPUT_JOYSTICK_H */
