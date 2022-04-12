#ifndef _WORLD_H
#define _WORLD_H

#include <stdint.h>

#include "engine/includes/3dMath.h"

#define VIEW_DISTANCE 5

void initWorld();
void calcWorld(vec3* playerPos, uint32_t ticks);
void drawWorld();

uint8_t intersectsRayWorld(vec3* playerPos, vec3* playerRot, uint8_t* hit);

#endif