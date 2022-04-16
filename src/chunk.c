#include "chunk.h"

#include "engine/savegame.h"

#include "block.h"

void generateDrawData(Chunk* chunk)
{
    chunk->isEmpty = 1;
    chunk->drawList = glGenLists(1);
	glNewList(chunk->drawList, GL_COMPILE);
	glBegin(GL_QUADS);

    for(uint8_t i = 0; i < CHUNK_SIZE; i++)
    {
        for(uint8_t j = 0; j < CHUNK_SIZE; j++)
        {
            for(uint8_t k = 0; k < CHUNK_SIZE; k++)
            {
                if(CHUNK_BLOCK(chunk, i, j, k).type != BLOCK_AIR)
                {
                    chunk->isEmpty = 0;

                    //Simple block occlusion check
                    uint8_t occlusion = 0;
                    if((k + 1 < CHUNK_SIZE) && isFullBlock(CHUNK_BLOCK(chunk, i, j, k + 1).type))
                    {
                        occlusion |= BS_FRONT;
                    }
                    if((k > 0) && isFullBlock(CHUNK_BLOCK(chunk, i, j, k - 1).type))
                    {
                        occlusion |= BS_BACK;
                    }
                    if((i + 1 < CHUNK_SIZE) && isFullBlock(CHUNK_BLOCK(chunk, i + 1, j, k).type))
                    {
                        occlusion |= BS_RIGHT;
                    }
                    if((i > 0) && isFullBlock(CHUNK_BLOCK(chunk, i - 1, j, k).type))
                    {
                        occlusion |= BS_LEFT;
                    }
                    if((j + 1 < CHUNK_SIZE) && isFullBlock(CHUNK_BLOCK(chunk, i, j + 1, k).type))
                    {
                        occlusion |= BS_TOP;
                    }
                    if(((j > 0) && isFullBlock(CHUNK_BLOCK(chunk, i, j - 1, k).type)) ||
                        (chunk->position.y == 0 && j == 0)) //Special case: The bottom of the bottom block of the bottom chunk (bedrock) will never be seen
                    {
                        occlusion |= BS_BOTTOM;
                    }
                    drawBlock(&CHUNK_BLOCK(chunk, i, j, k), i, j, k, occlusion);
                }
            }
        }
    }

	glEnd();
	glEndList();
}

void calcChunk(Chunk* chunk, uint32_t ticks)
{
    if(chunk->modified)
    {
        chunk->modified = 0;
        //TODO
        //glDeleteList(chunk->drawList);
        generateDrawData(chunk);
    }
}

void drawChunk(Chunk* chunk)
{
    //TODO: Translate in world space?
    glCallList(chunk->drawList);
}

void destroyChunk(Chunk* chunk)
{
    glDeleteList(chunk->drawList);
}

void calcChunkAABB(Chunk* chunk)
{
    chunk->aabb.min.x = chunk->position.x * CHUNK_SIZE;
    chunk->aabb.min.y = chunk->position.y * CHUNK_SIZE;
    chunk->aabb.min.z = chunk->position.z * CHUNK_SIZE;
    chunk->aabb.max.x = chunk->position.x * CHUNK_SIZE + CHUNK_SIZE;
    chunk->aabb.max.y = chunk->position.y * CHUNK_SIZE + CHUNK_SIZE;
    chunk->aabb.max.z = chunk->position.z * CHUNK_SIZE + CHUNK_SIZE;
}

uint8_t intersectsRayChunk(Chunk* chunk, vec3* origin, vec3* direction, BlockPos* block, float* distance)
{
    if(chunk->isEmpty)
    {
        return 0;
    }

    //Check against chunk AABB
    if(!aabbIntersectsRay(&chunk->aabb, origin, direction, distance))
    {
        return 0;
    }

    uint8_t found = 0;
    BlockPos minBlock;
    minBlock.chunk = chunk->position;
    float minDistance = 512;

    //Check against individual block AABBs
    for(uint8_t i = 0; i < CHUNK_SIZE; i++)
    {
        for(uint8_t j = 0; j < CHUNK_SIZE; j++)
        {
            for(uint8_t k = 0; k < CHUNK_SIZE; k++)
            {
                if(CHUNK_BLOCK(chunk, i, j, k).type != BLOCK_AIR)
                {
                    //TODO: Create AABB by block type
                    vec3 min = {chunk->aabb.min.x + i, chunk->aabb.min.y + j, chunk->aabb.min.z + k};
                    AABB blockAABB = {.min = min, .max = (vec3) {min.x + BLOCK_SIZE, min.y + BLOCK_SIZE, min.z + BLOCK_SIZE}};
                    
                    if(aabbIntersectsRay(&blockAABB, origin, direction, distance))
                    {
                        found = 1;
                        if(*distance < minDistance)
                        {
                            minBlock.x = i;
                            minBlock.y = j;
                            minBlock.z = k;
                            minDistance = *distance;
                        }
                    }
                }
            }
        }
    }

    *block = minBlock;
    *distance = minDistance;
    return found;
}