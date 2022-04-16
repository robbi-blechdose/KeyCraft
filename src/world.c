#include "world.h"

#include "engine/image.h"

#include "chunk.h"
#include "worldgen.h"

#define WORLD_CHUNK(i, j, k) chunks[(i) + ((j) * VIEW_DISTANCE) + ((k) * VIEW_DISTANCE * VIEW_DISTANCE)]

ChunkPos chunkPos;
Chunk* chunks[VIEW_DISTANCE * VIEW_DISTANCE * VIEW_DISTANCE];

GLuint terrainTexture;

void initWorld()
{
    initWorldgen(0);
    terrainTexture = loadRGBTexture("res/tex/terrain.png");

    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                WORLD_CHUNK(i, j, k) = calloc(1, sizeof(Chunk));
                WORLD_CHUNK(i, j, k)->position = (ChunkPos) {i, j, k};
                generateChunk(WORLD_CHUNK(i, j, k));
            }
        }
    }
    chunkPos = (ChunkPos) {0, 0, 0};
}

void quitWorld()
{
    //TODO
}

typedef enum {
    SWP_FB,
    SWP_LR,
    SWP_TB
} SwapSide;

typedef enum {
    SWP_FORE,
    SWP_BACK
} SwapDir;

//TODO: Redo
void swapChunks(SwapSide side, SwapDir direction)
{
    uint8_t out, in;
    if(direction == SWP_FORE)
    {
        out = 0;
        in = VIEW_DISTANCE - 1;
    }
    else
    {
        out = VIEW_DISTANCE - 1;
        in = 0;
    }

    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            //Destroy chunk "behind" the player
            Chunk* toDestroy;
            if(side == SWP_FB)
            {
                toDestroy = WORLD_CHUNK(out, i, j);
            }
            else if(side == SWP_LR)
            {
                toDestroy = WORLD_CHUNK(i, j, out);
            }
            else
            {
                toDestroy = WORLD_CHUNK(i, j, out);
            }
            destroyChunk(toDestroy);
            free(toDestroy);

            //Shift other chunks
            if(direction == SWP_FORE)
            {
                for(uint8_t k = 0; k < VIEW_DISTANCE - 1; k++)
                {
                    if(side == SWP_FB)
                    {
                        WORLD_CHUNK(k, i, j) = WORLD_CHUNK(k + 1, i, j);
                    }
                    else if(side == SWP_LR)
                    {
                        WORLD_CHUNK(i, j, k) = WORLD_CHUNK(i, j, k + 1);
                    }
                    else
                    {
                        WORLD_CHUNK(i, j, k) = WORLD_CHUNK(i, j, k + 1);
                    }
                }
            }
            else
            {
                for(uint8_t k = VIEW_DISTANCE - 1; k > 0; k--)
                {
                    if(side == SWP_FB)
                    {
                        WORLD_CHUNK(k, i, j) = WORLD_CHUNK(k - 1, i, j);
                    }
                    else if(side == SWP_LR)
                    {
                        WORLD_CHUNK(i, j, k) = WORLD_CHUNK(i, j, k - 1);
                    }
                    else
                    {
                        WORLD_CHUNK(i, j, k) = WORLD_CHUNK(i, j, k - 1);
                    }
                }
            }

            //Load new chunk "in front of" player
            Chunk* newChunk = calloc(1, sizeof(Chunk));
            if(side == SWP_FB)
            {
                newChunk->position = (ChunkPos) {chunkPos.x + in, chunkPos.y + i, chunkPos.z + j};
                WORLD_CHUNK(in, i, j) = newChunk;
            }
            else if(side == SWP_LR)
            {
                newChunk->position = (ChunkPos) {chunkPos.x + i, chunkPos.y + j, chunkPos.z + in};
                WORLD_CHUNK(i, j, in) = newChunk;
            }
            else
            {
                newChunk->position = (ChunkPos) {chunkPos.x + i, chunkPos.y + j, chunkPos.z + in};
                WORLD_CHUNK(i, j, in) = newChunk;
            }
            generateChunk(newChunk);
        }
    }
}

void calcWorld(vec3* playerPos, uint32_t ticks)
{
    //Load chunks around the player in/out
    ChunkPos playerChunkPos = {
        (playerPos->x + (CHUNK_SIZE / 2)) / CHUNK_SIZE,
        (playerPos->y + (CHUNK_SIZE / 2)) / CHUNK_SIZE,
        (playerPos->z + (CHUNK_SIZE / 2)) / CHUNK_SIZE
    };

    if(playerChunkPos.x - chunkPos.x != 0)
    {
        if(playerChunkPos.x > chunkPos.x)
        {
            chunkPos.x++;
            swapChunks(SWP_FB, SWP_FORE);
        }
        else
        {
            chunkPos.x--;
            swapChunks(SWP_FB, SWP_BACK);
        }
    }
    if(playerChunkPos.z - chunkPos.z != 0)
    {
        if(playerChunkPos.z > chunkPos.z)
        {
            chunkPos.z++;
            swapChunks(SWP_LR, SWP_FORE);
        }
        else
        {
            chunkPos.z--;
            swapChunks(SWP_LR, SWP_BACK);
        }
    }
    //TODO: Z

    //printf("P: %d %d %d\n", playerChunkPos.x, playerChunkPos.y, playerChunkPos.z);
    //printf("W: %d %d %d\n", chunkPos.x, chunkPos.y, chunkPos.z);
    
    //TODO

    //Calculate visible chunks
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                calcChunk(WORLD_CHUNK(i, j, k), ticks);
            }
        }
    }
}

//Translation to make handling chunk indices easier (0 - VIEW_DISTANCE) while centering the player
#define VIEW_TRANSLATION ((VIEW_DISTANCE * CHUNK_SIZE) / 2.0f)

void drawWorld()
{
    glBindTexture(GL_TEXTURE_2D, terrainTexture);

    //Draw visible chunks
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
        {
            //Translating here and then adding to it in the inner loop saves a few push/pops
            glPushMatrix();
            glTranslatef((i + chunkPos.x) * CHUNK_SIZE - VIEW_TRANSLATION,
                        chunkPos.y * CHUNK_SIZE - VIEW_TRANSLATION,
                        (k + chunkPos.z) * CHUNK_SIZE - VIEW_TRANSLATION);

            for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
            {
                //Discard empty chunks
                if(!WORLD_CHUNK(i, j, k)->isEmpty)
                {
                    drawChunk(WORLD_CHUNK(i, j, k));
                }
                glTranslatef(0, CHUNK_SIZE, 0);
            }
            glPopMatrix();
        }
    }
}

uint8_t intersectsRayWorld(vec3* origin, vec3* direction, float* hit)
{
    uint8_t found = 0;
    float minDistance = 512;

    //Exclude outer chunks
    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                //Discard empty chunks
                if(!WORLD_CHUNK(i, j, k)->isEmpty)
                {
                    if(intersectsRayChunk(WORLD_CHUNK(i, j, k), origin, direction, hit))
                    {
                        found = 1;
                        if(*hit < minDistance)
                        {
                            minDistance = *hit;
                        }
                    }
                }
            }
        }
    }

    *hit = minDistance;
    return found;
}