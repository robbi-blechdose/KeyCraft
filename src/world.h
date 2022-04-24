#ifndef _WORLD_H
#define _WORLD_H

#include <stdint.h>

#include "engine/includes/3dMath.h"

#include "blockutils.h"
#include "block.h"
#include "aabb.h"

#define VIEW_DISTANCE 5

#define TICK_RATE 500

void initWorld();
void calcWorld(vec3* playerPos, uint32_t ticks);
void drawWorld(vec3* playerRotation);

Block* getWorldBlock(BlockPos* pos);
void setWorldBlock(BlockPos* pos, Block block);

AABBSide intersectsRayWorld(vec3* origin, vec3* direction, BlockPos* block, float* distance);

#endif