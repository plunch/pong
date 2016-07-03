#ifndef GLOBAL_H
#define GLOBAL_H
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define UNUSED(x) (void)(x)

#define printerr(x) do { fprintf(stderr, " %s:%i: %s\n", __FILE__, __LINE__, x); } while(0)

#define error(x) do { fprintf(stderr, "%s:%i: %s (%s)\n", __FILE__, __LINE__, x, strerror(errno)); } while(0)

#define sdl_error(x) do { fprintf(stderr, "%s:%i: %s (%s)\n", __FILE__, __LINE__, x, SDL_GetError()); } while(0)

#define die_sdl(x) do { sdl_error(x); exit(-1); } while(0)

#define die(x) do { printerr(x); exit(-1); } while(0)

#define ATTRIB(x)     __attribute__((x))
#define likely(x)     __builtin_expect(!!(x), 1)
#define unlikely(x)   __builtin_expect(!!(x), 0)

#define ARRAYLEN(x) (sizeof(x)/sizeof(*x))

#endif /* GLOBAL_H */
