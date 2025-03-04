#include "worldgen.h"

#include <stdlib.h>

#define FNL_IMPL
#include "../fk-engine-core/includes/FastNoiseLite.h"
#include "../fk-engine-core/util.h"

#include "biomes.h"

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
    randNoise.noise_type = FNL_NOISE_OPENSIMPLEX2;
    randNoise.fractal_type = FNL_FRACTAL_NONE;
    randNoise.frequency = 0.1f;
    randNoise.seed = seed;
    //Init FastNoiseLite (biome)
    biomeNoise = fnlCreateState();
    biomeNoise.noise_type = FNL_NOISE_CELLULAR;
    biomeNoise.fractal_type = FNL_FRACTAL_NONE;
    biomeNoise.cellular_distance_func = FNL_CELLULAR_DISTANCE_HYBRID;
    biomeNoise.cellular_return_type = FNL_CELLULAR_RETURN_VALUE_CELLVALUE;
    biomeNoise.frequency = 0.03f;
    biomeNoise.seed = seed;
}

//TODO: this isn't really very random...
float getNoiseRandScale(int16_t chunkX, int16_t chunkZ, uint8_t x, uint8_t z, float y, float scale)
{
    //Scale noise to be within 0 to 1
    return (fnlGetNoise3D(&randNoise, (chunkX * CHUNK_SIZE + x) * scale, y, (chunkZ * CHUNK_SIZE + z) * scale) + 1) / 2;
}

float getNoiseRand(int16_t chunkX, int16_t chunkZ, uint8_t x, uint8_t z, float y)
{
    return getNoiseRandScale(chunkX, chunkZ, x, z, y, 100);
}

void generateLowBlocks(Chunk* chunk, uint8_t i, uint8_t j, uint8_t k, uint8_t blockWorldY)
{
    //Chunk position
    int16_t x = chunk->position.x;
    int16_t y = chunk->position.y;
    int16_t z = chunk->position.z;

    if(blockWorldY == 0)
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

void generateChunkNormal(Chunk* chunk, uint8_t i, uint8_t j, uint8_t k, uint8_t height, uint8_t blockWorldY)
{
    //Chunk position
    int16_t x = chunk->position.x;
    int16_t y = chunk->position.y;
    int16_t z = chunk->position.z;
    
    //Generate lakes
    if(blockWorldY >= height && blockWorldY <= WATER_LEVEL)
    {
        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_WATER;
    }
    else if(blockWorldY == height)
    {
        float rand = getNoiseRand(x, z, i, k, RAND_FLOWER);
        if(rand < 0.01f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_FLOWER;
        }
        else if(rand > 0.05f && rand < 0.051f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_FLOWER;
            CHUNK_BLOCK(chunk, i, j, k).data = BLOCK_DATA_TEXTURE1;
        }
        else if(getNoiseRand(x, z, i, k, RAND_TALLGRASS) < 0.02f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_TALL_GRASS;
        }
    }
    else if(blockWorldY == height - 1)
    {
        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_GRASS;
        //Generate lake surroundings
        if(blockWorldY <= WATER_LEVEL)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_SAND;
        }
    }
    else if(blockWorldY < height && blockWorldY >= height * 0.7f)
    {
        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_DIRT;
    }
    else if(blockWorldY < height * 0.7f)
    {
        generateLowBlocks(chunk, i, j, k, blockWorldY);
    }

    //TODO: fix chance here
    //Generate sugar canes
    if(blockWorldY > WATER_LEVEL)
    {
        int8_t sugarcaneHeight = getNoiseRand(x, z, i, k, RAND_SUGARCANE) * 30 - 5;
        sugarcaneHeight = sugarcaneHeight <= MAX_SUGAR_CANE_HEIGHT ? sugarcaneHeight : MAX_SUGAR_CANE_HEIGHT;

        if(blockWorldY <= WATER_LEVEL + sugarcaneHeight && blockWorldY >= height && height == WATER_LEVEL + 1) //&& blockWorldY > WATER_LEVEL
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_SUGAR_CANE;
            CHUNK_BLOCK(chunk, i, j, k).data = 0; //Clear data - it can happen that flowers are placed which causes the texture bit to be set
        }
    }
}

void generateChunkDesert(Chunk* chunk, uint8_t i, uint8_t j, uint8_t k, uint8_t height, uint8_t blockWorldY)
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

    if(blockWorldY == height)
    {
        if(getNoiseRand(x, z, i, k, RAND_FLOWER) < 0.02f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_DEAD_SHRUB;
        }
    }
    else if(blockWorldY < height && blockWorldY >= height * 0.7f)
    {
        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_SAND;
    }
    else if(blockWorldY < height * 0.7f)
    {
        generateLowBlocks(chunk, i, j, k, blockWorldY);
    }

    //Generate cacti
    int8_t cactusHeight = getNoiseRandScale(x, z, i, k, RAND_CACTUS, 50) * 35 - 30;
    cactusHeight = cactusHeight <= MAX_CACTUS_HEIGHT ? cactusHeight : MAX_CACTUS_HEIGHT;

    if(blockWorldY >= height && blockWorldY < height + cactusHeight)
    {
        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_CACTUS;
    }
}

void generateChunkBarren(Chunk* chunk, uint8_t i, uint8_t j, uint8_t k, uint8_t height, uint8_t blockWorldY)
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

    if(blockWorldY == height)
    {
        float rand = getNoiseRand(x, z, i, k, RAND_FLOWER);
        if(rand < 0.2f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_DEAD_SHRUB;
        }
        else if(rand > 0.5f && rand < 0.55f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_MUSHROOM;
            if(rand > 0.525f)
            {
                CHUNK_BLOCK(chunk, i, j, k).data = BLOCK_DATA_TEXTURE1;
            }
        }
    }
    else if(blockWorldY < height && blockWorldY >= height * 0.8f)
    {
        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_DIRT;
    }
    else if(blockWorldY < height * 0.8f)
    {
        generateLowBlocks(chunk, i, j, k, blockWorldY);
    }
}

void generateChunkRocks(Chunk* chunk, uint8_t i, uint8_t j, uint8_t k, uint8_t height, uint8_t blockWorldY)
{
    //Chunk position
    int16_t x = chunk->position.x;
    int16_t y = chunk->position.y;
    int16_t z = chunk->position.z;
    
    //Generate lakes
    if(blockWorldY >= height && blockWorldY <= WATER_LEVEL)
    {
        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_LAVA;
    }
    else if(blockWorldY == height)
    {
        float rand = getNoiseRand(x, z, i, k, RAND_FLOWER);
        if(rand < 0.4f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_MOSS;
        }
    }
    else if(blockWorldY < height && blockWorldY >= height * 0.7f)
    {
        if(getNoiseRand(x, z, i, k, RAND_ROCKS) < 0.1f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_STONE;
        }
        else
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_COBBLESTONE;
        }
    }
    else if(blockWorldY < height * 0.7f)
    {
        generateLowBlocks(chunk, i, j, k, blockWorldY);
    }
}

void generateChunkForest(Chunk* chunk, uint8_t i, uint8_t j, uint8_t k, uint8_t height, uint8_t blockWorldY)
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

    //Generate lakes
    if(blockWorldY == height)
    {
        if(getNoiseRand(x, z, i, k, RAND_TALLGRASS) < 0.4f)
        {
            CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_TALL_GRASS;
        }
    }
    else if(blockWorldY == height - 1)
    {
        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_GRASS;
    }
    else if(blockWorldY < height && blockWorldY >= height * 0.7f)
    {
        CHUNK_BLOCK(chunk, i, j, k).type = BLOCK_DIRT;
    }
    else if(blockWorldY < height * 0.7f)
    {
        generateLowBlocks(chunk, i, j, k, blockWorldY);
    }
}

BiomeType getBiome(int16_t chunkX, int16_t chunkZ, uint8_t x, uint8_t z)
{
    float biome = (fnlGetNoise2D(&biomeNoise, chunkX * CHUNK_SIZE + x, chunkZ * CHUNK_SIZE + z) + 1) / 2;

    //Special case: normal biome is the "default", so skip it in the loop and ignore its spawn interval
    for(uint8_t i = 1; i < NUM_BIOME_TYPES; i++)
    {
        if(biome > biomeDefinitions[i].spawnChance.start && biome < biomeDefinitions[i].spawnChance.end)
        {
            return i;
        }
    }

    return BIOME_NORMAL;
}

//TODO: move into biome data?
typedef void (*generateChunkFunction)(Chunk*, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

generateChunkFunction generateChunkFunctions[] = {
    [BIOME_NORMAL] = generateChunkNormal,
    [BIOME_DESERT] = generateChunkDesert,
    [BIOME_BARREN] = generateChunkBarren,
    [BIOME_ROCKS]  = generateChunkRocks,
    [BIOME_FOREST] = generateChunkForest
};

uint8_t getHeight(int16_t chunkX, int16_t chunkZ, uint8_t x, uint8_t z)
{
    //Noise value (-1 to 1) is scaled to be within 0 to 20 (+1 to bring it to 0 to 2, *10 to bring it to 0 to 20)
    return (fnlGetNoise2D(&terrainNoise, (chunkX * CHUNK_SIZE + x) * 4, (chunkZ * CHUNK_SIZE + z) * 4) + 1) * 10;
}

void generateChunk(Chunk* chunk)
{
    //Chunk position
    int16_t x = chunk->position.x;
    int16_t y = chunk->position.y;
    int16_t z = chunk->position.z;

    //Clearing the chunk is unnecessary since data is allocated with calloc()

    //Generate base chunk
    for(uint8_t i = 0; i < CHUNK_SIZE; i++)
    {
        for(uint8_t k = 0; k < CHUNK_SIZE; k++)
        {
            uint8_t height = getHeight(x, z, i, k);

            for(uint8_t j = 0; j < CHUNK_SIZE; j++)
            {
                uint8_t blockWorldY = j + (y * CHUNK_SIZE);

                generateChunkFunctions[getBiome(x, z, i, k)](chunk, i, j, k, height, blockWorldY);
            }
        }
    }

    //Decorate chunk: Create structure data
    //Get biome for center of chunk (sort of, actual center would be 3.5)
    BiomeType biome = getBiome(x, z, 4, 4);
    for(uint8_t i = 0; i < biomeDefinitions[biome].numStructureSpawnData; i++)
    {
        if(getNoiseRand(x, z, 4, 4, biomeDefinitions[biome].structureSpawnData->randomNoiseY) > biomeDefinitions[biome].structureSpawnData->spawnChance)
        {
            continue;
        }

        uint8_t baseX = getNoiseRand(x, z, 0, 0, biomeDefinitions[biome].structureSpawnData->randomNoiseY) * CHUNK_SIZE;
        uint8_t baseZ = getNoiseRand(x, z, 8, 8, biomeDefinitions[biome].structureSpawnData->randomNoiseY) * CHUNK_SIZE;

        int16_t groundHeight = getHeight(x, z, baseX, baseZ) -  (y * CHUNK_SIZE) - 1; //-1 since the generators use this as the ground level

        if(groundHeight >= CHUNK_SIZE || groundHeight < 0)
        {
            //Skip if chunk is either full, or we cannot detect the block below the tree
            //Or if we're more than one chunk above the ground
            break;
        }

        uint8_t spawnOnBlockX = structureDefinitions[biomeDefinitions[biome].structureSpawnData->type].spawnOnBlockPosX;
        uint8_t spawnOnBlockZ = structureDefinitions[biomeDefinitions[biome].structureSpawnData->type].spawnOnBlockPosZ;
        BlockType spawnOnBlockType = structureDefinitions[biomeDefinitions[biome].structureSpawnData->type].spawnOnBlockType;

        if(CHUNK_BLOCK(chunk, baseX + spawnOnBlockX, groundHeight, baseZ + spawnOnBlockZ).type != spawnOnBlockType)
        {
            //Block below isn't grass, skip
            break;
        }

        //TODO: do we want to be able to place multiple structures?
        if(chunk->numStructures < MAX_STRUCTURES)
        {
            chunk->structureData[chunk->numStructures++] = (StructureData) {.type = biomeDefinitions[biome].structureSpawnData->type,
                                                                            .isSource = true,
                                                                            .x = baseX,
                                                                            .y = groundHeight + 1,
                                                                            .z = baseZ};
        }
    }
    
    //Calculate basic AABB
    calcChunkAABB(chunk);

    //We've generated structure data
    CHUNK_SET_FLAG(chunk, CHUNK_NEW_STRUCT_DATA);
    //Mark chunk as initial - has been generated, no player-made modifications
    CHUNK_SET_FLAG(chunk, CHUNK_IS_INITIAL);
    //Don't mark the chunk to be built yet - we're not done!
}

void propagateChunkStructureData(Chunk* sourceChunk, Chunk** adjacentChunks, uint8_t numAdjacentChunks)
{
    for(uint8_t i = 0; i < sourceChunk->numStructures; i++)
    {
        if(!sourceChunk->structureData[i].isSource)
        {
            continue;
        }

        AABB structureAABB = getAABBForStructure(&sourceChunk->structureData[i]);
        moveAABB(&structureAABB, sourceChunk->aabb.min);
        
        if(aabbInsideAABB(&structureAABB, &sourceChunk->aabb))
        {
            //Easy case: structure is entirely inside this chunk
            continue;
        }

        for(uint8_t j = 0; j < numAdjacentChunks; j++)
        {
            //Skip the source chunk if it's in the list
            //Also skip any chunks that have been player-modified
            if(adjacentChunks[j] == sourceChunk || !CHUNK_GET_FLAG(adjacentChunks[j], CHUNK_IS_INITIAL))
            {
                continue;
            }

            if(!aabbIntersectsAABB(&structureAABB, &adjacentChunks[j]->aabb))
            {
                continue;
            }
                
            //TODO: what do we do if this is already full? can we at least make that very unlikely?
            if(adjacentChunks[j]->numStructures < MAX_STRUCTURES)
            {
                adjacentChunks[j]->structureData[adjacentChunks[j]->numStructures].type = sourceChunk->structureData[i].type;
                adjacentChunks[j]->structureData[adjacentChunks[j]->numStructures].isSource = false;

                int16_t xDiff = (sourceChunk->position.x - adjacentChunks[j]->position.x) * CHUNK_SIZE;
                int16_t yDiff = (sourceChunk->position.y - adjacentChunks[j]->position.y) * CHUNK_SIZE;
                int16_t zDiff = (sourceChunk->position.z - adjacentChunks[j]->position.z) * CHUNK_SIZE;

                adjacentChunks[j]->structureData[adjacentChunks[j]->numStructures].x = sourceChunk->structureData[i].x + xDiff;
                adjacentChunks[j]->structureData[adjacentChunks[j]->numStructures].y = sourceChunk->structureData[i].y + yDiff;
                adjacentChunks[j]->structureData[adjacentChunks[j]->numStructures].z = sourceChunk->structureData[i].z + zDiff;
                adjacentChunks[j]->numStructures++;

                CHUNK_SET_FLAG(adjacentChunks[j], CHUNK_NEW_STRUCT_DATA);
            }
        }
    }
}

//TODO: add option to generate "base plate" below structure! (necessary for volcanoes, possibly others)
void generateStructurePart(Chunk* chunk, StructureData* structure)
{
    //Offset the starting point of the structure in general (where is the structure placed)
    vec3u8 offset;
    //Start of structure generation, relative to the structure's own origin
    vec3u8 start;

    //Assign values based on structure position (since the origin is a corner of the structure, negative values must mean the origin is outside of this chunk)
    offset.x = structure->x >= 0 ? structure->x : 0;
    start.x  = structure->x >= 0 ? 0 : -structure->x;

    offset.y = structure->y >= 0 ? structure->y : 0;
    start.y  = structure->y >= 0 ? 0 : -structure->y;

    offset.z = structure->z >= 0 ? structure->z : 0;
    start.z  = structure->z >= 0 ? 0 : -structure->z;

    //Grab definition
    StructureDefinition structureDef = structureDefinitions[structure->type];

    //Copy relevant part of structure into chunk
    for(uint8_t j = start.y; j < structureDef.size.y; j++)
    {
        uint8_t y = offset.y + j - start.y;
        if(y >= CHUNK_SIZE)
        {
            return;
        }
        
        for(uint8_t i = start.x; i < structureDef.size.x; i++)
        {
            uint8_t x = offset.x + i - start.x;
            if(x >= CHUNK_SIZE)
            {
                continue;
            }
        
            for(uint8_t k = start.z; k < structureDef.size.z; k++)
            {
                uint8_t z = offset.z + k - start.z;
                if(z >= CHUNK_SIZE)
                {
                    continue;
                }

                //TODO: check if the multiplications here work for non-square structures!
                Block newBlock = structureDef.blocks[j * structureDef.size.x * structureDef.size.z + k * structureDef.size.x + i];
                //Don't clear out terrain around structures
                if(newBlock.type != BLOCK_AIR)
                {
                    CHUNK_BLOCK(chunk, x, y, z) = newBlock;
                }
            }
        }
    }
}

void generateChunkStructures(Chunk* chunk)
{
    if(!CHUNK_GET_FLAG(chunk, CHUNK_NEW_STRUCT_DATA))
    {
        return;
    }

    //Generate structure (parts) belonging to this chunk
    for(uint8_t i = 0; i < chunk->numStructures; i++)
    {
        generateStructurePart(chunk, &chunk->structureData[i]);
    }

    //*Now* we're done with this chunk
    //All structures have been generated
    CHUNK_CLEAR_FLAG(chunk, CHUNK_NEW_STRUCT_DATA);
    //Mark chunk geometry to be built
    CHUNK_SET_FLAG(chunk, CHUNK_MODIFIED);
    CHUNK_SET_FLAG(chunk, CHUNK_NO_DRAW_DATA);
    //Mark chunk as initial - has been generated, no player-made modifications
    CHUNK_SET_FLAG(chunk, CHUNK_IS_INITIAL);
}
