#ifndef RENDERIMPL_H
#define RENDERIMPL_H

#include <stdlib.h>
#include "render.h"

#define RENDER_EMPTY {\
	.userdata     = NULL, \
	.drawtext     = NULL, \
	.outputbounds = NULL, \
	.clear        = NULL, \
	.flip         = NULL, \
	.draw         = NULL, \
	.load         = NULL, \
	.destroy      = NULL, \
}

struct renderer {
	void* userdata;
	size_t (*drawtext)(void*, 
	                   enum render_text_alignment,
			   enum render_text_style,
	                   int, // x
	                   int, // y
	                   int, // w
	                   int, // h
	                   const char*);
	int (*outputbounds)(void*, int*, int*); // w, h
	void (*clear)(void*);
	void (*flip)(void*);
	void (*draw)(void*,
	             struct render_item*,
	             int,  // x
	             int,  // y
	             int,  // w
	             int); // h
	struct render_item* (*load)(void*, const char*);
	void (*destroy)(void*, struct render_item*);
};

#endif /* RENDERIMPL_H */
