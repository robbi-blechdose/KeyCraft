#ifndef _WORLD_H
#define _WORLD_H

#include <stdint.h>

#include "engine/includes/3dMath.h"

#include "blockutils.h"
#include "block.h"

#define VIEW_DISTANCE 5

void initWorld();
void calcWorld(vec3* playerPos, uint32_t ticks);
void drawWorld();

Block* getWorldBlock(BlockPos* pos);
void setWorldBlock(BlockPos* pos, uint8_t type);

uint8_t intersectsRayWorld(vec3* origin, vec3* direction, BlockPos* block, float* distance);

#endif