#ifndef _CHUNK_H
#define _CHUNK_H

#include <GL/gl.h>

#include "engine/includes/3dMath.h"

#include "block.h"
#include "aabb.h"

#define CHUNK_SIZE 8

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} ChunkPos;

typedef struct {
    AABB aabb;
    ChunkPos position;
    Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
    uint8_t isEmpty;
    uint8_t modified;
    GLuint drawList;
} Chunk;

#define CHUNK_BLOCK(chunk, i, j, k) chunk->blocks[(i) + ((j) * CHUNK_SIZE) + ((k) * CHUNK_SIZE * CHUNK_SIZE)]

void calcChunk(Chunk* chunk, uint32_t ticks);
void drawChunk(Chunk* chunk);

void destroyChunk(Chunk* chunk);

uint8_t intersectsRayChunk(Chunk* chunk, vec3* origin, vec3* direction, float* hit);

#endif