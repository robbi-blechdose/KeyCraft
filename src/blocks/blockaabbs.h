#ifndef BLOCK_AABBS_H
#define BLOCK_AABBS_H

#include "block.h"
#include "../aabb.h"

AABB getAABBForBlock(Block* block, vec3 position);

#endif