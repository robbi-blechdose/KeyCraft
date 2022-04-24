#ifndef _CHUNK_H
#define _CHUNK_H

#include <GL/gl.h>

#include "engine/includes/3dMath.h"

#include "block.h"
#include "aabb.h"
#include "blockutils.h"

#define CHUNK_SIZE 8

typedef struct {
    AABB aabb;
    ChunkPos position;
    Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
    uint8_t isEmpty;
    uint8_t modified;
    GLuint drawList;
} Chunk;

#define CHUNK_BLOCK(chunk, i, j, k) chunk->blocks[(i) + ((j) * CHUNK_SIZE) + ((k) * CHUNK_SIZE * CHUNK_SIZE)]

#define CHUNK_MODIFIED_INITIAL 0b10000000

void calcChunk(Chunk* chunk);

void drawChunk(Chunk* chunk);

void destroyChunk(Chunk* chunk);

void calcChunkAABB(Chunk* chunk);
AABBSide intersectsRayChunk(Chunk* chunk, vec3* origin, vec3* direction, BlockPos* block, float* distance);

#endif