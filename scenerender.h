#ifndef SCENERENDER_H
#define SCENERENDER_H


#include "render.h"
#include "simulation.h"

void draw_scene(struct scene* scene,
                struct renderer* renderer,
                struct render_item* paddle1,
                struct render_item* paddle2,
                struct render_item* ball,
                struct render_item* field);

#endif /* SCENERENDER_H */
