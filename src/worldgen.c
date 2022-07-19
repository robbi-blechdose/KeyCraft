#include "worldgen.h"

#include <stdlib.h>

#define FNL_IMPL
#include "engine/includes/FastNoiseLite.h"
#include "engine/util.h"

#define TREE_SIZE             5
#define TREE_HEIGHT           6
#define TREE_LEAVES_START     2

#define MAX_SUGAR_CANE_HEIGHT 3
#define MAX_CACTUS_HEIGHT     3

#define RAND_FLOWER         0
#define RAND_TALLGRASS      8
#define RAND_TREE          16
#define RAND_SUGARCANE     24
#define RAND_CACTUS        24

#define RAND_ORE_COAL     128
#define RAND_ORE_IRON     144
#define RAND_ORE_GOLD     160
#define RAND_ORE_REDSTONE 176
#define RAND_ORE_DIAMOND  192

#define WATER_LEVEL 6

fnl_state terrainNoise;
fnl_state randNoise;
fnl_state biomeNoise;

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
    //Init FastNoiseLite (biome)
    biomeNoise = fnlCreateState();
    biomeNoise.noise_type = FNL_NOISE_CELLULAR;
    biomeNoise.fractal_type = FNL_FRACTAL_NONE;
    biomeNoise.cellular_distance_func = FNL_CELLULAR_DISTANCE_HYBRID;
    biomeNoise.seed = seed;
}

float getNoiseRand(int16_t chunkX, int16_t chunkZ, uint8_t x, uint8_t z, float y)
{
    //Scale noise to be within 0 to 1
    return (fnlGetNoise3D(&randNoise, (chunkX * CHUNK_SIZE + x) * 50, y, (chunkZ * CHUNK_SIZE + z) * 50) + 1) / 2;
}

void generateTree(Chunk* chunk, uint8_t baseX, uint8_t baseY, uint8_t baseZ)
{
    uint8_t height = TREE_HEIGHT - getNoiseRand(chunk->position.x, chunk->position.z, 4, 4, RAND_TREE) * (TREE_HEIGHT / 3);
    uint8_t leavesStart = TREE_LEAVES_START + getNoiseRand(chunk->position.x, chunk->position.z, 4, 4, RAND_TREE) * (TREE_LEAVES_START / 2);

    for(uint8_t j = baseY; j < baseY + height; j++)
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
                        !(j == baseY + height - 1 && (i <= baseX || i >= baseX + TREE_SIZE - 1 || k <= baseZ || k >= baseZ + TREE_SIZE - 1)))
                    {
                        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_LEAVES;
                    }
                }
            }
        }
        //Generate trunk
        if(j < baseY + height - 1)
        {
            CHUNK_BLOCK(chunk, baseX + (TREE_SIZE / 2), j, baseZ + (TREE_SIZE / 2)).type = BLOCK_WOOD;
        }
    }
}

void generateLowBlocks(Chunk* chunk, uint8_t i, uint8_t j, uint8_t k, uint8_t pos)
{
    //Chunk position
    int16_t x = chunk->position.x;
    int16_t y = chunk->position.y;
    int16_t z = chunk->position.z;

    if(pos == 0)
    {
        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_BEDROCK;
    }
    else
    {
        if(getNoiseRand(x, z, i, k, RAND_ORE_COAL + j * 4) < 0.05f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_IRON_ORE;
        }
        else if(getNoiseRand(x, z, i, k, RAND_ORE_IRON + j * 4) < 0.03f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_COAL_ORE;
        }
        else if(getNoiseRand(x, z, i, k, RAND_ORE_GOLD + j * 4) < 0.015f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_GOLD_ORE;
        }
        else if(getNoiseRand(x, z, i, k, RAND_ORE_REDSTONE + j * 4) < 0.015f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_REDSTONE_ORE;
        }
        else if(getNoiseRand(x, z, i, k, RAND_ORE_DIAMOND + j * 4) < 0.01f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_DIAMOND_ORE;
        }
        else
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_STONE;
        }
    }
}

void generateChunkNormal(Chunk* chunk, uint8_t i, uint8_t j, uint8_t k, uint8_t height, uint8_t pos)
{
    //Chunk position
    int16_t x = chunk->position.x;
    int16_t y = chunk->position.y;
    int16_t z = chunk->position.z;

    if(pos == height)
    {
        float rand = getNoiseRand(x, z, i, k, RAND_FLOWER);
        if(rand < 0.01f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_FLOWER;
            if(rand < 0.007f)
            {
                CHUNK_BLOCK(chunk, i, j, k).data = BLOCK_DATA_TEXTURE1;
            }
        }
        else if(getNoiseRand(x, z, i, k, RAND_TALLGRASS) < 0.02f)
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
        generateLowBlocks(chunk, i, j, k, pos);
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
    //Generate sugar canes
    else
    {
        int8_t sugarcaneHeight = getNoiseRand(x, z, i, k, RAND_SUGARCANE) * 30 - 5;
        sugarcaneHeight = sugarcaneHeight <= MAX_SUGAR_CANE_HEIGHT ? sugarcaneHeight : MAX_SUGAR_CANE_HEIGHT;

        if(pos <= WATER_LEVEL + sugarcaneHeight && pos >= height && height == WATER_LEVEL + 1) //&& pos > WATER_LEVEL (implicitly true because of the else above)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_SUGAR_CANE;
        }
    }
}

void generateChunkDesert(Chunk* chunk, uint8_t i, uint8_t j, uint8_t k, uint8_t height, uint8_t pos)
{
    //Chunk position
    int16_t x = chunk->position.x;
    int16_t y = chunk->position.y;
    int16_t z = chunk->position.z;

    //Clamp to water level to prevent issues on biome transitions
    if(height <= WATER_LEVEL)
    {
        height = WATER_LEVEL + 1;
    }

    if(pos == height)
    {
        if(getNoiseRand(x, z, i, k, RAND_FLOWER) < 0.02f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_DEAD_SHRUB;
        }
    }
    else if(pos < height && pos >= height * 0.7f)
    {
        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_SAND;
    }
    else if(pos < height * 0.7f)
    {
        generateLowBlocks(chunk, i, j, k, pos);
    }

    //Generate cacti
    int8_t cactusHeight = getNoiseRand(x, z, i, k, RAND_CACTUS) * 60 - 20;
    cactusHeight = cactusHeight <= MAX_CACTUS_HEIGHT ? cactusHeight : MAX_CACTUS_HEIGHT;

    if(pos >= height && pos < height + cactusHeight)
    {
        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_CACTUS;
    }
}

void generateChunk(Chunk* chunk)
{
    //Chunk position
    int16_t x = chunk->position.x;
    int16_t y = chunk->position.y;
    int16_t z = chunk->position.z;

    //Clear chunk
    memset(chunk->blocks, 0, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * sizeof(Block));

    //Generate base chunk
    for(uint8_t i = 0; i < CHUNK_SIZE; i++)
    {
        for(uint8_t k = 0; k < CHUNK_SIZE; k++)
        {
            //Noise value (-1 to 1) is scaled to be within 0 to 20
            uint8_t height = (fnlGetNoise2D(&terrainNoise, (x * CHUNK_SIZE + i) * 4, (z * CHUNK_SIZE + k) * 4) + 1) * 10;

            for(uint8_t j = 0; j < CHUNK_SIZE; j++)
            {
                uint8_t pos = j + (y * CHUNK_SIZE);

                float biome = (fnlGetNoise2D(&biomeNoise, x * CHUNK_SIZE + i, z * CHUNK_SIZE + k) + 1) / 2;

                if(biome < 0.15f)
                {
                    generateChunkDesert(chunk, i, j, k, height, pos);
                }
                else
                {
                    generateChunkNormal(chunk, i, j, k, height, pos);
                }
            }
        }
    }

    //Decorate chunk (trees etc.)
    if(getNoiseRand(x, z, 4, 4, RAND_TREE) < 0.06f)
    {
        uint8_t baseX = getNoiseRand(x, z, 0, 0, RAND_TREE) * (CHUNK_SIZE - TREE_SIZE);
        uint8_t baseZ = getNoiseRand(x, z, 8, 8, RAND_TREE) * (CHUNK_SIZE - TREE_SIZE);

        uint8_t baseY = 255;
        for(uint8_t j = 0; j < CHUNK_SIZE; j++)
        {
            if(CHUNK_BLOCK(chunk, baseX + (TREE_SIZE / 2), j, baseZ + (TREE_SIZE / 2)).type == BLOCK_GRASS)
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
    CHUNK_SET_FLAG(chunk, CHUNK_MODIFIED);
    CHUNK_SET_FLAG(chunk, CHUNK_MODIFIED_INITIAL);
    //Mark chunk as initial - has been generated, no player-made modifications
    CHUNK_SET_FLAG(chunk, CHUNK_IS_INITIAL);

    //Calculate basic AABB
    calcChunkAABB(chunk);
}