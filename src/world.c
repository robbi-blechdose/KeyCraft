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

void swapChunks(uint8_t side, uint8_t direction)
{
    uint8_t out, in;
    if(direction)
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
            if(side == 0)
            {
                toDestroy = WORLD_CHUNK(out, i, j);
            }
            else if(side == 1)
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
            if(direction)
            {
                for(uint8_t k = 0; k < VIEW_DISTANCE - 1; k++)
                {
                    if(side == 0)
                    {
                        WORLD_CHUNK(k, i, j) = WORLD_CHUNK(k + 1, i, j);
                    }
                    else if(side == 1)
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
                    if(side == 0)
                    {
                        WORLD_CHUNK(k, i, j) = WORLD_CHUNK(k - 1, i, j);
                    }
                    else if(side == 1)
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
            if(side == 0)
            {
                WORLD_CHUNK(in, i, j) = newChunk;
                generateChunk(newChunk, chunkPos.x + in, chunkPos.y + j, chunkPos.z + i);
            }
            else if(side == 1)
            {
                WORLD_CHUNK(i, in, j) = newChunk;
                generateChunk(newChunk, chunkPos.x + i, chunkPos.y + in, chunkPos.z + j);
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
            swapChunks(0, 1);
            /**
            for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
            {
                for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
                {
                    destroyChunk(WORLD_CHUNK(0, j, k));
                    free(WORLD_CHUNK(0, j, k));
                    for(uint8_t i = 0; i < VIEW_DISTANCE - 1; i++)
                    {
                        WORLD_CHUNK(i, j, k) = WORLD_CHUNK(i + 1, j, k);
                    }
                    WORLD_CHUNK(VIEW_DISTANCE - 1, j, k) = calloc(1, sizeof(Chunk));
                    generateChunk(WORLD_CHUNK(VIEW_DISTANCE - 1, j, k), chunkPos.x + VIEW_DISTANCE - 1, chunkPos.y + k, chunkPos.z + j);
                }
            }**/
        }
        else
        {
            chunkPos.x--;
            swapChunks(0, 0);
            /**
            for(uint8_t j = 0; j < VIEW_DISTANCE; j++)
            {
                for(uint8_t k = 0; k < VIEW_DISTANCE; k++)
                {
                    destroyChunk(WORLD_CHUNK(VIEW_DISTANCE - 1, j, k));
                    free(WORLD_CHUNK(VIEW_DISTANCE - 1, j, k));
                    for(uint8_t i = VIEW_DISTANCE - 1; i > 0; i--)
                    {
                        WORLD_CHUNK(i, j, k) = WORLD_CHUNK(i - 1, j, k);
                    }
                    WORLD_CHUNK(0, j, k) = calloc(1, sizeof(Chunk));
                    generateChunk(WORLD_CHUNK(0, j, k), chunkPos.x + 0, chunkPos.y + k, chunkPos.z + j);
                }
            }**/
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