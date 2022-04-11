#ifndef _WORLD_H
#define _WORLD_H

#include <stdint.h>

#include "engine/includes/3dMath.h"

#define VIEW_DISTANCE 5

void initWorld();
void calcWorld(vec3* playerPos, uint32_t ticks);
void drawWorld();

#endif