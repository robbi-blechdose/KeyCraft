#include "worldgen.h"

#include <stdlib.h>

#define FNL_IMPL
#include "engine/includes/FastNoiseLite.h"
#include "engine/util.h"

#define TREE_SIZE 5
#define TREE_HEIGHT 6
#define TREE_LEAVES_START 3

#define RAND_FLOWER   0
#define RAND_TREE     8
#define RAND_ORE    128

#define WATER_LEVEL 6

fnl_state terrainNoise;
fnl_state randNoise;

void initWorldgen(uint32_t seed)
{
    //Init FastNoiseLite (terrain)
    terrainNoise = fnlCreateState();
    terrainNoise.noise_type = FNL_NOISE_PERLIN;
    terrainNoise.fractal_type = FNL_FRACTAL_NONE;
    terrainNoise.seed = seed;
    //Init FastNoiseLite (random)
    randNoise = fnlCreateState();
    randNoise.noise_type = FNL_NOISE_CELLULAR;
    randNoise.fractal_type = FNL_FRACTAL_NONE;
    randNoise.seed = seed;
}

float getNoiseRand(int16_t chunkX, int16_t chunkZ, uint8_t x, uint8_t z, uint8_t y)
{
    //Scale noise to be within 0 to 1
    return (fnlGetNoise3D(&randNoise, (chunkX * CHUNK_SIZE + x) * 10, y, (chunkZ * CHUNK_SIZE + z) * 10) + 1) / 2;
}

void generateTree(Chunk* chunk, uint8_t baseX, uint8_t baseY, uint8_t baseZ)
{
    for(uint8_t j = baseY; j < baseY + TREE_HEIGHT; j++)
    {
        //Generate leaves
        if(j > TREE_LEAVES_START)
        {
            for(uint8_t i = baseX; i < baseX + TREE_SIZE; i++)
            {
                for(uint8_t k = baseZ; k < baseZ + TREE_SIZE; k++)
                {
                    //Exclude corners, exclude outer ring on top layer
                    if(!((i == baseX || i == baseX + TREE_SIZE - 1) && (k == baseZ || k == baseZ + TREE_SIZE - 1)) &&
                        !(j == baseY + TREE_HEIGHT - 1 && (i <= baseX || i >= baseX + TREE_SIZE - 1 || k <= baseZ || k >= baseZ + TREE_SIZE - 1)))
                    {
                        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_LEAVES;
                    }
                }
            }
        }
        //Generate trunk
        if(j < baseY + TREE_HEIGHT - 1)
        {
            CHUNK_BLOCK(chunk, baseX + (TREE_SIZE / 2), j, baseZ + (TREE_SIZE / 2)).type = BLOCK_WOOD;
        }
    }
}

void generateChunk(Chunk* chunk)
{
    //Chunk position
    int16_t x = chunk->position.x;
    int16_t y = chunk->position.y;
    int16_t z = chunk->position.z;

    for(uint8_t i = 0; i < CHUNK_SIZE; i++)
    {
        for(uint8_t k = 0; k < CHUNK_SIZE; k++)
        {
            //Noise value (-1 to 1) is scaled to be within 0 to 20
            uint8_t height = (fnlGetNoise2D(&terrainNoise, (x * CHUNK_SIZE + i) * 4, (z * CHUNK_SIZE + k) * 4) + 1) * 10;

            for(uint8_t j = 0; j < CHUNK_SIZE; j++)
            {
                uint8_t pos = j + (y * CHUNK_SIZE);
                
                if(pos == height)
                {
                    float rand = getNoiseRand(x, z, i, k, RAND_FLOWER);
                    if(rand < 0.01f)
                    {
                        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_FLOWER;
                        if(rand < 0.005f)
                        {
                            CHUNK_BLOCK(chunk, i, j, k).data = BLOCK_DATA_TEXTURE1;
                        }
                    }
                    else if(rand > 0.5f)
                    {
                        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_TALL_GRASS;
                    }
                }
                else if(pos == height - 1)
                {
                    CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_GRASS;
                }
                else if(pos < height && pos >= height * 0.7f)
                {
                    CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_DIRT;
                }
                else if(pos < height * 0.7f)
                {
                    if(pos == 0)
                    {
                        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_BEDROCK;
                    }
                    else
                    {
                        float rand = getNoiseRand(x, z, i, k, RAND_ORE);

                        if(rand < 0.05f)
                        {
                            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_IRON_ORE;
                        }
                        else if(rand < 0.1f)
                        {
                            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_COAL_ORE;
                        }
                        else
                        {
                            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_STONE;
                        }
                    }
                }

                //Generate lakes
                if(pos <= WATER_LEVEL)
                {
                    if(pos >= height)
                    {
                        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_WATER;
                    }
                    else if(pos == height - 1)
                    {
                        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_SAND;
                    }
                }
            }
        }
    }

    //Place tree?
    if(getNoiseRand(x, z, 4, 4, RAND_TREE) < 0.05f)
    {
        uint8_t baseX = getNoiseRand(x, z, 0, 0, RAND_TREE) * (CHUNK_SIZE - TREE_SIZE);
        uint8_t baseZ = getNoiseRand(x, z, 8, 8, RAND_TREE) * (CHUNK_SIZE - TREE_SIZE);

        uint8_t baseY = 255;
        for(uint8_t j = 0; j < CHUNK_SIZE; j++)
        {
            //TODO: Also exclude placement on tall grass or flowers
            if(CHUNK_BLOCK(chunk, baseX + (TREE_SIZE / 2), j, baseZ + (TREE_SIZE / 2)).type != BLOCK_AIR)
            {
                baseY = j + 1;
            }
        }
        if(baseY <= CHUNK_SIZE - TREE_HEIGHT)
        {
            generateTree(chunk, baseX, baseY, baseZ);
        }
    }

    //Mark chunk geometry to be built
    chunk->modified = CHUNK_MODIFIED_INITIAL;
    //Mark chunk as initial - has been generated, no player-made modifications
    chunk->initial = 1;

    //Calculate basic AABB
    calcChunkAABB(chunk);
}