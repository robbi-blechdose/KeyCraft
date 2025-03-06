#ifndef CHUNK_H
#define CHUNK_H

#include <stdbool.h>
#include <GL/gl.h>

#include "fk-engine-core/includes/3dMath.h"

#include "blocks/block.h"
#include "aabb.h"
#include "blocks/blockutils.h"
#include "computer.h"
#include "worldgen/structures.h"

#include "saves.h"

#define CHUNK_SIZE 8
#define NUM_COMPUTERS 8
#define MAX_STRUCTURES 4

typedef struct {
    AABB aabb;
    ChunkPos position;
    Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
    ComputerData* computers[NUM_COMPUTERS];
    GLuint drawList;
    //Flags
    uint8_t flags;
    //Structure data (only relevant for chunk generation)
    StructureData structureData[MAX_STRUCTURES];
    uint8_t numStructures;
} Chunk;

#define CHUNK_BLOCK(chunk, i, j, k) chunk->blocks[(i) + ((j) * CHUNK_SIZE) + ((k) * CHUNK_SIZE * CHUNK_SIZE)]

#define CHUNK_GET_FLAG(chunk, flag) (chunk->flags & (flag))
#define CHUNK_SET_FLAG(chunk, flag) (chunk->flags |= (flag))
#define CHUNK_CLEAR_FLAG(chunk, flag) (chunk->flags &= ~(flag))

//Chunk contains no blocks
#define CHUNK_IS_EMPTY         0b00000001
//Chunk hasn't been modified from its generated state
#define CHUNK_IS_INITIAL       0b00000010
//Chunk hasn't had its geometry generation done yet
#define CHUNK_NO_DRAW_DATA     0b00000100
//Chunk has received new structure data
#define CHUNK_NEW_STRUCT_DATA  0b00001000
//Chunk has been modified in some way and requires geometry re-generation
#define CHUNK_MODIFIED         0b01000000

void calcChunk(Chunk* chunk);

void drawChunk(Chunk* chunk);

void destroyChunk(Chunk* chunk);

void calcChunkAABB(Chunk* chunk);
AABBSide intersectsRayChunk(Chunk* chunk, vec3* origin, vec3* direction, BlockPos* block, float* distance);
bool intersectsAABBChunk(Chunk* chunk, AABB* aabb);

void saveChunk(Chunk* chunk);
void loadChunk(Chunk* chunk, SaveVersionCompat svc);

#endif