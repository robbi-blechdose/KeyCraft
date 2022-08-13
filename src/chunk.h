#ifndef _CHUNK_H
#define _CHUNK_H

#include <stdbool.h>
#include <GL/gl.h>

#include "engine/includes/3dMath.h"

#include "blocks/block.h"
#include "aabb.h"
#include "blocks/blockutils.h"
#include "computer.h"

#define CHUNK_SIZE 8
#define NUM_COMPUTERS 8

typedef struct {
    AABB aabb;
    ChunkPos position;
    Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
    ComputerData* computers[NUM_COMPUTERS];
    GLuint drawList;
    //Flags
    uint8_t flags;
} Chunk;

#define CHUNK_BLOCK(chunk, i, j, k) chunk->blocks[(i) + ((j) * CHUNK_SIZE) + ((k) * CHUNK_SIZE * CHUNK_SIZE)]

#define CHUNK_GET_FLAG(chunk, flag) (chunk->flags & (flag))
#define CHUNK_SET_FLAG(chunk, flag) (chunk->flags |= (flag))
#define CHUNK_CLEAR_FLAG(chunk, flag) (chunk->flags &= ~(flag))

#define CHUNK_IS_EMPTY         0b00000001
#define CHUNK_IS_INITIAL       0b00000010
#define CHUNK_MODIFIED         0b01000000
#define CHUNK_MODIFIED_INITIAL 0b10000000

void calcChunk(Chunk* chunk);

void drawChunk(Chunk* chunk);

void destroyChunk(Chunk* chunk);

void calcChunkAABB(Chunk* chunk);
AABBSide intersectsRayChunk(Chunk* chunk, vec3* origin, vec3* direction, BlockPos* block, float* distance);
bool intersectsAABBChunk(Chunk* chunk, AABB* aabb);

void saveChunk(Chunk* chunk);
void loadChunk(Chunk* chunk);

#endif