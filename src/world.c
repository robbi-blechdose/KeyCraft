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
    initWorldgen();
    terrainTexture = loadRGBTexture("res/tex/terrain.png");

    for(uint8_t i = 0; i < VIEW_DISTANCE; i++)
    {
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                WORLD_CHUNK(i, j, k) = calloc(1, sizeof(Chunk));
                generateChunk(WORLD_CHUNK(i, j, k), i, k, j);
            }
        }
    }
    chunkPos = (ChunkPos) {0, 0, 0};
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
                toDestroy = WORLD_CHUNK(i, out, j);
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
                        WORLD_CHUNK(i, k, j) = WORLD_CHUNK(i, k + 1, j);
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
                        WORLD_CHUNK(i, k, j) = WORLD_CHUNK(i, k - 1, j);
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
                WORLD_CHUNK(in, i, j) = newChunk;
                generateChunk(newChunk, chunkPos.x + in, chunkPos.y + j, chunkPos.z + i);
            }
            else if(side == SWP_LR)
            {
                WORLD_CHUNK(i, in, j) = newChunk;
                generateChunk(newChunk, chunkPos.x + i, chunkPos.y + j, chunkPos.z + in);
            }
            else
            {
                WORLD_CHUNK(i, j, in) = newChunk;
                generateChunk(newChunk, chunkPos.x + i, chunkPos.y + j, chunkPos.z + in);
            }
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
        for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
        {
            for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
            {
                glPushMatrix();
                glTranslatef((i + chunkPos.x) * CHUNK_SIZE - VIEW_TRANSLATION,
                             (k + chunkPos.y) * CHUNK_SIZE - VIEW_TRANSLATION,
                             (j + chunkPos.z) * CHUNK_SIZE - VIEW_TRANSLATION);
                drawChunk(WORLD_CHUNK(i, j, k));
                glPopMatrix();
            }
        }
    }
}

void quitWorld()
{
    //TODO
}