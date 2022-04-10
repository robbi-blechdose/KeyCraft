#ifndef _WORLD_H
#define _WORLD_H

#include <stdint.h>

#include "engine/includes/3dMath.h"

#define VIEW_DISTANCE 5

#define WORLD_HEIGHT 5

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} ChunkPos;

void initWorld();
void calcWorld(vec3* playerPos, uint32_t ticks);
void drawWorld();

#endif