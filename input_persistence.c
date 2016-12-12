#include "input_persistence.h"

#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "input/impl/sdl.h"

#define BUFZ 512

int input_state_write(struct input_state* state, SDL_RWops* rw)
{
	assert(state != NULL);
	assert(rw != NULL);

	char buf[BUFZ];

	for(unsigned i = 0; i < state->num_mappings; ++i) {
		memset(buf, '\0', BUFZ);

		int wr = snprintf(buf, BUFZ, "%u ",
		                  state->mappings[i].intent);
		if (wr <= 0)
			return 0;

		if (SDL_RWwrite(rw, buf, sizeof(char), (size_t)wr)
		      < (size_t)wr)
			return 0;

		memset(buf, '\0', (size_t)wr);

		wr = input_sdl_write_id(state->mappings[i].source,
		                        buf, BUFZ);

		if (wr <= 0)
			return 0;

		if (SDL_RWwrite(rw, buf, sizeof(char), (size_t)wr)
		      < (size_t)wr)
			return 0;

		if (!SDL_RWwrite(rw, "\n", sizeof(char), 1))
			return 0;
	}

	return 1;
}

int input_state_read(struct input_state* state, SDL_RWops* rw)
{
	assert(state != NULL);
	assert(rw != NULL);

	char buf[BUFZ];
	size_t len;
	int iseof = 0;

	while(!iseof) {
		len = 0;
		while (len < BUFZ) {
			if (!SDL_RWread(rw, buf + len, 1, 1)) {
				const char* sdl_error = SDL_GetError();
				if (sdl_error != NULL && sdl_error[0] == '\0')
					iseof = 1;

				if (!iseof) {
					SDL_SetError("%s", sdl_error);
					return 0;
				}
			}
			if (buf[len] == '\n') {
				buf[len] = '\0';
				if (len > 0 && buf[len-1] == '\r')
					buf[--len] = '\0';
				break;
			}
			len++;
		}
		if (len == BUFZ && buf[len] != '\0')
			return 0;

		char* more;
		unsigned long intent = strtoul(buf, &more, 0);
		if (more <= buf)
			return 0;
		len -= (size_t)(more - buf);
		if (len > 0 && more[0] == ' ') {
			more++;
			len--;
		}

		struct input_source_id id = input_sdl_read_id(more, len);

		unsigned inte = intent > UINT_MAX
		                       ? UINT_MAX 
		                       : (unsigned)intent;

		if (!input_state_add_mapping(state, id, inte))
			return 0;
	}

	return 1;
}
