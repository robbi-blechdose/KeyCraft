#ifndef _WORLD_H
#define _WORLD_H

#include <stdint.h>

#include "engine/includes/3dMath.h"

#include "blockutils.h"
#include "block.h"
#include "aabb.h"
#include "chunk.h"

#define VIEW_DISTANCE 5

#define MAX_RAY_DISTANCE 5

#define TICK_RATE 250

void initWorld();
void calcWorld(vec3* playerPos, uint32_t ticks);
void drawWorld(vec3* playerPosition, vec3* playerRotation);

Block* getWorldBlock(BlockPos* pos);
void setWorldBlock(BlockPos* pos, Block block);

AABBSide intersectsRayWorld(vec3* origin, vec3* direction, BlockPos* block, float* distance);
uint8_t intersectsAABBWorld(AABB* aabb);

#endif