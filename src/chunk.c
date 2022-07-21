#include "chunk.h"

#include "engine/savegame.h"

#include "world.h"
#include "blocks/blockaabbs.h"

uint8_t isWorldBlockOpaque(Chunk* chunk, uint8_t i, uint8_t j, uint8_t k)
{
    BlockPos testPos = {.chunk = chunk->position, .x = i, .y = j, .z = k};
    Block* testBlock = getWorldBlock(&testPos);
    if(testBlock == NULL)
    {
        return 1;
    }
    else if(isOpaqueBlock(testBlock->type))
    {
        return 1;
    }
    return 0;
}

uint8_t getOcclusionForBlock(Chunk* chunk, uint8_t i, uint8_t j, uint8_t k)
{
    uint8_t occlusion = 0;

    if((k + 1 < CHUNK_SIZE))
    {
        if(isOpaqueBlock(CHUNK_BLOCK(chunk, i, j, k + 1).type))
        {
            occlusion |= BS_FRONT;
        }
    }
    else if(isWorldBlockOpaque(chunk, i, j, k + 1))
    {
        occlusion |= BS_FRONT;
    }

    if((k > 0))
    {
        if(isOpaqueBlock(CHUNK_BLOCK(chunk, i, j, k - 1).type))
        {
            occlusion |= BS_BACK;
        }
    }
    else if(isWorldBlockOpaque(chunk, i, j, k - 1))
    {
        occlusion |= BS_BACK;
    }

    if((i + 1 < CHUNK_SIZE))
    {
        if(isOpaqueBlock(CHUNK_BLOCK(chunk, i + 1, j, k).type))
        {
            occlusion |= BS_RIGHT;
        }
    }
    else if(isWorldBlockOpaque(chunk, i + 1, j, k))
    {
        occlusion |= BS_RIGHT;
    }

    if((i > 0))
    {
        if(isOpaqueBlock(CHUNK_BLOCK(chunk, i - 1, j, k).type))
        {
            occlusion |= BS_LEFT;
        }
    }
    else if(isWorldBlockOpaque(chunk, i - 1, j, k))
    {
        occlusion |= BS_LEFT;
    }

    if((j + 1 < CHUNK_SIZE))
    {
        if(isOpaqueBlock(CHUNK_BLOCK(chunk, i, j + 1, k).type))
        {
            occlusion |= BS_TOP;
        }
    }
    else if(isWorldBlockOpaque(chunk, i, j + 1, k))
    {
        occlusion |= BS_TOP;
    }

    if(j > 0)
    {
        if(isOpaqueBlock(CHUNK_BLOCK(chunk, i, j - 1, k).type))
        {
            occlusion |= BS_BOTTOM;
        }
    }
    else if(isWorldBlockOpaque(chunk, i, j - 1, k))
    {
        occlusion |= BS_BOTTOM;
    }

    return occlusion;
}

void generateDrawData(Chunk* chunk)
{
    CHUNK_SET_FLAG(chunk, CHUNK_IS_EMPTY);
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
                    CHUNK_CLEAR_FLAG(chunk, CHUNK_IS_EMPTY);

                    uint8_t occlusion = getOcclusionForBlock(chunk, i, j, k);

                    if(occlusion != BS_ALL)
                    {
                        drawBlock(&CHUNK_BLOCK(chunk, i, j, k), i, j, k, occlusion);
                    }
                }
            }
        }
    }

	glEnd();
	glEndList();
}

void calcChunk(Chunk* chunk)
{
    if(CHUNK_GET_FLAG(chunk, CHUNK_MODIFIED))
    {
        if(!CHUNK_GET_FLAG(chunk, CHUNK_MODIFIED_INITIAL))
        {
            glDeleteList(chunk->drawList);
            CHUNK_CLEAR_FLAG(chunk, CHUNK_MODIFIED_INITIAL);
        }
        generateDrawData(chunk);
        CHUNK_CLEAR_FLAG(chunk, CHUNK_MODIFIED);
    }
}

void drawChunk(Chunk* chunk)
{
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

AABBSide intersectsRayChunk(Chunk* chunk, vec3* origin, vec3* direction, BlockPos* block, float* distance)
{
    if(CHUNK_GET_FLAG(chunk, CHUNK_IS_EMPTY))
    {
        return 0;
    }

    //Check against chunk AABB
    if(!aabbIntersectsRay(&chunk->aabb, origin, direction, distance))
    {
        return 0;
    }

    AABBSide minSide = AABB_NONE;
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
                    AABBSide result = aabbIntersectsRay(&blockAABB, origin, direction, distance);

                    if(result != AABB_NONE)
                    {
                        if(*distance < minDistance)
                        {
                            minSide = result;
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
    return minSide;
}

bool intersectsAABBChunk(Chunk* chunk, AABB* aabb)
{
    if(CHUNK_GET_FLAG(chunk, CHUNK_IS_EMPTY))
    {
        return false;
    }

    //Check against chunk AABB
    if(!aabbIntersectsAABB(&chunk->aabb, aabb))
    {
        return false;
    }

    //Check against individual block AABBs
    for(uint8_t i = 0; i < CHUNK_SIZE; i++)
    {
        for(uint8_t j = 0; j < CHUNK_SIZE; j++)
        {
            for(uint8_t k = 0; k < CHUNK_SIZE; k++)
            {
                if(CHUNK_BLOCK(chunk, i, j, k).type != BLOCK_AIR && isBlockCollidable(CHUNK_BLOCK(chunk, i, j, k).type))
                {
                    vec3 min = {chunk->aabb.min.x + i, chunk->aabb.min.y + j, chunk->aabb.min.z + k};
                    AABB blockAABB = getAABBForBlock(&CHUNK_BLOCK(chunk, i, j, k), min);

                    if(aabbIntersectsAABB(&blockAABB, aabb))
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void saveChunk(Chunk* chunk)
{
    writeElement(&chunk->position, sizeof(ChunkPos));
    writeElement(&chunk->blocks, sizeof(Block) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);

    uint8_t numComputers = 0;
    for(uint8_t i = 0; i < NUM_COMPUTERS; i++)
    {
        if(chunk->computers[i] != NULL)
        {
            numComputers++;
        }
    }
    writeElement(&numComputers, sizeof(uint8_t));
    for(uint8_t i = 0; i < NUM_COMPUTERS; i++)
    {
        if(chunk->computers[i] != NULL)
        {
            writeElement(&i, sizeof(uint8_t));
            writeElement(&chunk->computers[i]->program, PROGRAM_SIZE * sizeof(uint8_t));
        }
    }
}

void loadChunk(Chunk* chunk)
{
    readElement(&chunk->position, sizeof(ChunkPos));
    readElement(&chunk->blocks, sizeof(Block) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);

    uint8_t numComputers;
    readElement(&numComputers, sizeof(uint8_t));
    for(uint8_t i = 0; i < numComputers; i++)
    {
        uint8_t index;
        readElement(&index, sizeof(uint8_t));
        chunk->computers[i] = createComputer();
        readElement(&chunk->computers[i]->program, PROGRAM_SIZE * sizeof(uint8_t));
    }

    //Since we loaded the chunk it's been player-modified (otherwise saving + loading doesn't take place)
    CHUNK_CLEAR_FLAG(chunk, CHUNK_IS_INITIAL);
    //Mark to build geometry
    CHUNK_SET_FLAG(chunk, CHUNK_MODIFIED);
    CHUNK_SET_FLAG(chunk, CHUNK_MODIFIED_INITIAL);
    //Calculate basic AABB
    calcChunkAABB(chunk);
}