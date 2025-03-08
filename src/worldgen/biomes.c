#include "biomes.h"

#include "worldnoise.h"
#include "../blocks/block.h"

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

#define RAND_ROCKS        256
#define RAND_VOLCANO      264

#define WATER_LEVEL 6

//TODO: we now use this in 3 places, unify!
#define B(X) (Block) {(X), 0}

Block generateLowBlocks(uint8_t j, uint8_t blockWorldY)
{
    if(blockWorldY == 0)
    {
        return B(BLOCK_BEDROCK);
    }

    if(getNoiseRand(RAND_ORE_COAL + j * 4) < 0.05f)
    {
        return B(BLOCK_IRON_ORE);
    }
    else if(getNoiseRand(RAND_ORE_IRON + j * 4) < 0.03f)
    {
        return B(BLOCK_COAL_ORE);
    }
    else if(getNoiseRand(RAND_ORE_GOLD + j * 4) < 0.015f)
    {
        return B(BLOCK_GOLD_ORE);
    }
    else if(getNoiseRand(RAND_ORE_REDSTONE + j * 4) < 0.015f)
    {
        return B(BLOCK_REDSTONE_ORE);
    }
    else if(getNoiseRand(RAND_ORE_DIAMOND + j * 4) < 0.01f)
    {
        return B(BLOCK_DIAMOND_ORE);
    }
    else
    {
        return B(BLOCK_STONE);
    }
}

Block generateBiomeNormal(uint8_t j, uint8_t height, uint8_t blockWorldY)
{
    //Generate lakes
    if(blockWorldY >= height && blockWorldY <= WATER_LEVEL)
    {
        return B(BLOCK_WATER);
    }
    else if(blockWorldY == height)
    {
        float rand = getNoiseRand(RAND_FLOWER);
        if(rand < 0.01f)
        {
            return B(BLOCK_FLOWER);
        }
        else if(rand > 0.05f && rand < 0.051f)
        {
            return (Block) {BLOCK_FLOWER, BLOCK_DATA_TEXTURE1};
        }
        else if(getNoiseRand(RAND_TALLGRASS) < 0.02f)
        {
            return B(BLOCK_TALL_GRASS);
        }
    }
    else if(blockWorldY == height - 1)
    {
        return B(BLOCK_GRASS);
        //Generate lake surroundings
        if(blockWorldY <= WATER_LEVEL)
        {
            return B(BLOCK_SAND);
        }
    }
    else if(blockWorldY < height && blockWorldY >= height * 0.7f)
    {
        return B(BLOCK_DIRT);
    }
    else if(blockWorldY < height * 0.7f)
    {
        return generateLowBlocks(j, blockWorldY);
    }

    //TODO: fix chance here
    //Generate sugar canes
    if(blockWorldY > WATER_LEVEL)
    {
        int8_t sugarcaneHeight = getNoiseRand(RAND_SUGARCANE) * 30 - 5;
        sugarcaneHeight = sugarcaneHeight <= MAX_SUGAR_CANE_HEIGHT ? sugarcaneHeight : MAX_SUGAR_CANE_HEIGHT;

        if(blockWorldY <= WATER_LEVEL + sugarcaneHeight && blockWorldY >= height && height == WATER_LEVEL + 1) //&& blockWorldY > WATER_LEVEL
        {
            return B(BLOCK_SUGAR_CANE);
        }
    }

    return B(BLOCK_AIR);
}

Block generateBiomeDesert(uint8_t j, uint8_t height, uint8_t blockWorldY)
{
    //Clamp to water level to prevent issues on biome transitions
    if(height <= WATER_LEVEL)
    {
        height = WATER_LEVEL + 1;
    }

    if(blockWorldY == height)
    {
        if(getNoiseRand(RAND_FLOWER) < 0.08f)
        {
            return B(BLOCK_DEAD_SHRUB);
        }
    }
    else if(blockWorldY < height && blockWorldY >= height * 0.7f)
    {
        return B(BLOCK_SAND);
    }
    else if(blockWorldY < height * 0.7f)
    {
        return generateLowBlocks(j, blockWorldY);
    }

    //Generate cacti
    int8_t cactusHeight = getNoiseRandScale(RAND_CACTUS, 50) * 35 - 30;
    cactusHeight = cactusHeight <= MAX_CACTUS_HEIGHT ? cactusHeight : MAX_CACTUS_HEIGHT;

    if(blockWorldY >= height && blockWorldY < height + cactusHeight)
    {
        return B(BLOCK_CACTUS);
    }

    return B(BLOCK_AIR);
}

Block generateBiomeBarren(uint8_t j, uint8_t height, uint8_t blockWorldY)
{
    //Clamp to water level to prevent issues on biome transitions
    if(height <= WATER_LEVEL)
    {
        height = WATER_LEVEL + 1;
    }

    if(blockWorldY == height)
    {
        float rand = getNoiseRand(RAND_FLOWER);
        if(rand < 0.15f)
        {
            return B(BLOCK_DEAD_SHRUB);
        }
        else if(rand > 0.5f && rand < 0.525f)
        {
            if(rand > 0.5125f)
            {
                return (Block) {BLOCK_MUSHROOM, BLOCK_DATA_TEXTURE1};
            }
            return B(BLOCK_MUSHROOM);
        }
    }
    else if(blockWorldY < height && blockWorldY >= height * 0.8f)
    {
        return B(BLOCK_DIRT);
    }
    else if(blockWorldY < height * 0.8f)
    {
        return generateLowBlocks(j, blockWorldY);
    }

    return B(BLOCK_AIR);
}

Block generateBiomeRocks(uint8_t j, uint8_t height, uint8_t blockWorldY)
{
    //Generate lakes
    if(blockWorldY >= height && blockWorldY <= WATER_LEVEL)
    {
        return B(BLOCK_LAVA);
    }
    else if(blockWorldY == height)
    {
        float rand = getNoiseRand(RAND_FLOWER);
        if(rand < 0.4f)
        {
            return B(BLOCK_MOSS);
        }
    }
    else if(blockWorldY < height && blockWorldY >= height * 0.7f)
    {
        if(getNoiseRand(RAND_ROCKS) < 0.1f)
        {
            return B(BLOCK_STONE);
        }
        else
        {
            return B(BLOCK_COBBLESTONE);
        }
    }
    else if(blockWorldY < height * 0.7f)
    {
        return generateLowBlocks(j, blockWorldY);
    }

    return B(BLOCK_AIR);
}

Block generateBiomeForest(uint8_t j, uint8_t height, uint8_t blockWorldY)
{
    //Clamp to water level to prevent issues on biome transitions
    if(height <= WATER_LEVEL)
    {
        height = WATER_LEVEL + 1;
    }

    //Generate lakes
    if(blockWorldY == height)
    {
        if(getNoiseRand(RAND_TALLGRASS) < 0.4f)
        {
            return B(BLOCK_TALL_GRASS);
        }
    }
    else if(blockWorldY == height - 1)
    {
        return B(BLOCK_GRASS);
    }
    else if(blockWorldY < height && blockWorldY >= height * 0.7f)
    {
        return B(BLOCK_DIRT);
    }
    else if(blockWorldY < height * 0.7f)
    {
        return generateLowBlocks(j, blockWorldY);
    }

    return B(BLOCK_AIR);
}

BiomeDefinition biomeDefinitions[NUM_BIOME_TYPES] = {
    [BIOME_NORMAL] = {
        //Since this is the "default" case, we don't need spawn chance data
        .numStructureSpawnData = 1,
        .structureSpawnData = (struct StructureSpawnData[]) {
            {
                .type = STRUCTURE_TREE,
                .randomNoiseY = 16,
                .spawnChance = 1
            }
        },
        .generator = generateBiomeNormal
    },
    [BIOME_DESERT] = {
        .spawnChance = {.start = 0.0f, .end = 0.2f},
        .numStructureSpawnData = 0,
        .generator = generateBiomeDesert
    },
    [BIOME_BARREN] = {
        .spawnChance = {.start = 0.8f, .end = 1.0f},
        .numStructureSpawnData = 1,
        .structureSpawnData = (struct StructureSpawnData[]) {
            {
                .type = STRUCTURE_ROCK_PILE,
                .randomNoiseY = 24,
                .spawnChance = 1
            }
        },
        .generator = generateBiomeBarren
    },
    [BIOME_ROCKS] = {
        .spawnChance = {.start = 0.3f, .end = 0.35f},
        .numStructureSpawnData = 1,
        .structureSpawnData = (struct StructureSpawnData[]) {
            {
                .type = STRUCTURE_VOLCANO,
                .randomNoiseY = 16,
                .spawnChance = 1
            }
        },
        .generator = generateBiomeRocks
    },
    [BIOME_FOREST] = {
        .spawnChance = {.start = 0.4f, .end = 0.5f},
        .numStructureSpawnData = 1,
        .structureSpawnData = (struct StructureSpawnData[]) {
            {
                .type = STRUCTURE_TREE_TALL,
                .randomNoiseY = 16,
                .spawnChance = 1
            }
        },
        .generator = generateBiomeForest
    }
};