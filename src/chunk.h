#ifndef _CHUNK_H
#define _CHUNK_H

#include <GL/gl.h>

#include "engine/includes/3dMath.h"

#include "blocks/block.h"
#include "aabb.h"
#include "blocks/blockutils.h"

#define CHUNK_SIZE 8

typedef struct {
    AABB aabb;
    ChunkPos position;
    Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
    GLuint drawList;
    //Flags
    uint8_t isEmpty;
    uint8_t modified;
    uint8_t initial;
} Chunk;

#define CHUNK_BLOCK(chunk, i, j, k) chunk->blocks[(i) + ((j) * CHUNK_SIZE) + ((k) * CHUNK_SIZE * CHUNK_SIZE)]

#define CHUNK_MODIFIED_INITIAL 0b10000000

void calcChunk(Chunk* chunk);

void drawChunk(Chunk* chunk);

void destroyChunk(Chunk* chunk);

void calcChunkAABB(Chunk* chunk);
AABBSide intersectsRayChunk(Chunk* chunk, vec3* origin, vec3* direction, BlockPos* block, float* distance);
uint8_t intersectsAABBChunk(Chunk* chunk, AABB* aabb);

void saveChunk(Chunk* chunk);
void loadChunk(Chunk* chunk);

#endif