#ifndef WORLD_H
#define WORLD_H

#include <stdbool.h>
#include <stdint.h>

#include "engine/includes/3dMath.h"

#include "blocks/blockutils.h"
#include "blocks/block.h"
#include "aabb.h"
#include "chunk.h"

//View distance in chunks
#define VIEW_DISTANCE 5
//Boundaries for chunks for block picking (3x5x3 chunks, outer x and z chunks can be excluded since the player is always in (or close to) the center chunk)
#define MIN_RAY_CHUNK 1
#define MAX_RAY_CHUNK 3
//Max distance of a ray for block picking, in blocks
#define MAX_RAY_DISTANCE 5
//Rate for game logic, in ms
#define TICK_RATE 250
//Maximum number of chunks to recalculate geometry for per frame
#define MAX_CHUNKS_PER_FRAME 5

//Translation to make handling chunk indices easier (0 - VIEW_DISTANCE) while centering the player
#define VIEW_TRANSLATION ((VIEW_DISTANCE * CHUNK_SIZE) / 2)

void initWorld(uint32_t seed);
void quitWorld();

void calcWorld(vec3* playerPos, uint32_t ticks);
void drawWorld(vec3* playerPosition, vec3* playerRotation);

void normalizeBlockPos(BlockPos* pos);

Chunk* getWorldChunk(BlockPos* pos);
Block* getWorldBlock(BlockPos* pos);
void setWorldBlock(BlockPos* pos, Block block);
bool actWorldBlock(BlockPos* pos);

AABBSide intersectsRayWorld(vec3* origin, vec3* direction, BlockPos* block, float* distance);
bool intersectsAABBWorld(AABB* aabb);

bool isWorldUnmodified();
void saveWorld();
void loadWorld();

#endif