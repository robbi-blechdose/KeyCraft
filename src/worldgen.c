#include "worldgen.h"

#include <stdlib.h>

#define FNL_IMPL
#include "engine/includes/FastNoiseLite.h"
#include "engine/util.h"

//TODO: Switch all random usage to second noise - that way, values are reproducible and depend on the seed

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

#define TREE_SIZE 5
#define TREE_HEIGHT 6
#define TREE_LEAVES_START 3

float getNoiseRand(int16_t chunkX, int16_t chunkZ, uint8_t x, uint8_t z)
{
    //Scale noise to be within 0 to 1
    return (fnlGetNoise2D(&randNoise, (chunkX * CHUNK_SIZE + x) * 10, (chunkZ * CHUNK_SIZE + z) * 10) + 1) / 2;
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
                    float rand = getNoiseRand(x, z, i, k);
                    if(rand < 0.005f)
                    {
                        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_FLOWER;
                        if(rand < 0.0025f)
                        {
                            CHUNK_BLOCK(chunk, i, j, k).data = BLOCK_DATA_TYPE;
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
                else if(pos < height && pos >= height / 2)
                {
                    CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_DIRT;
                }
                else if(pos < height / 2)
                {
                    if(pos == 0)
                    {
                        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_BEDROCK;
                    }
                    else
                    {
                        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_STONE;
                    }
                }
            }
        }
    }

    //Place tree?
    if(getNoiseRand(x, z, 4, 4) < 0.05f)
    {
        uint8_t baseX = getNoiseRand(x, z, 0, 0) * (CHUNK_SIZE - TREE_SIZE);
        uint8_t baseZ = getNoiseRand(x, z, 8, 8) * (CHUNK_SIZE - TREE_SIZE);

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
    chunk->modified = 1;

    //Calculate basic AABB
    calcChunkAABB(chunk);
}