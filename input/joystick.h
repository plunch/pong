#ifndef INPUT_JOYSTICK_H
#define INPUT_JOYSTICK_H

#include "../input.h"
#include "../inputsource_sdl.h"

#include <SDL2/SDL.h>

struct joyaxis_mapping {
	SDL_JoystickID joystick;
	Uint8 axis;
	Sint16 deadzone;
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
	Uint8 ppos, npos;
	enum game_action action;
};

struct joyball_mapping {
	SDL_JoystickID joystick;
	Uint8 ball;
	unsigned int horizontal : 1;
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
