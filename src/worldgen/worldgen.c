#include "worldgen.h"

#include <stdlib.h>

#include "../fk-engine-core/util.h"

#include "biomes.h"
#include "worldnoise.h"

void initWorldgen(uint32_t seed)
{
    initWorldNoise(seed);
}

BiomeType getBiome(int16_t chunkX, int16_t chunkZ, uint8_t x, uint8_t z)
{
    setNoisePosition(chunkX, chunkZ, x, z);
    float biome = getNoiseBiome();

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

uint8_t getHeight(int16_t chunkX, int16_t chunkZ, uint8_t x, uint8_t z)
{
    //Noise value is scaled to be within 0 to 20
    setNoisePosition(chunkX, chunkZ, x, z);
    return getNoiseTerrain() * 20;
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
            setNoisePosition(x, z, i, k);

            for(uint8_t j = 0; j < CHUNK_SIZE; j++)
            {
                //j is in chunk-local space, jWorld is the same coordinate in world space
                uint8_t jWorld = j + (y * CHUNK_SIZE);
                BiomeType biome = getBiome(x, z, i, k);
                CHUNK_BLOCK(chunk, i, j, k) = biomeDefinitions[biome].generator(j, height, jWorld);
            }
        }
    }

    //Decorate chunk: Create structure data
    //Get biome for center of chunk (sort of, actual center would be 3.5)
    BiomeType biome = getBiome(x, z, 4, 4);
    for(uint8_t i = 0; i < biomeDefinitions[biome].numStructureSpawnData; i++)
    {
        struct StructureSpawnData* structureSpawnData = &biomeDefinitions[biome].structureSpawnData[i];

        setNoisePosition(x, z, 4, 4);
        if(getNoiseRand(structureSpawnData->randomNoiseY) > structureSpawnData->spawnChance)
        {
            continue;
        }

        setNoisePosition(x, z, 0, 0);
        uint8_t baseX = getNoiseRand(structureSpawnData->randomNoiseY) * CHUNK_SIZE;
        setNoisePosition(x, z, 8, 8);
        uint8_t baseZ = getNoiseRand(structureSpawnData->randomNoiseY) * CHUNK_SIZE;

        int16_t groundHeight = getHeight(x, z, baseX, baseZ) -  (y * CHUNK_SIZE) - 1; //-1 since the generators use this as the ground level

        if(groundHeight >= CHUNK_SIZE || groundHeight < 0)
        {
            //Skip if chunk is either full, or we cannot detect the block below the tree
            //Or if we're more than one chunk above the ground
            break;
        }

        uint8_t spawnOnBlockX = structureDefinitions[structureSpawnData->type].spawnOnBlockPosX;
        uint8_t spawnOnBlockZ = structureDefinitions[structureSpawnData->type].spawnOnBlockPosZ;
        BlockType spawnOnBlockType = structureDefinitions[structureSpawnData->type].spawnOnBlockType;

        if(spawnOnBlockType != BLOCK_AIR &&
            CHUNK_BLOCK(chunk, baseX + spawnOnBlockX, groundHeight, baseZ + spawnOnBlockZ).type != spawnOnBlockType)
        {
            //Block below isn't correct, skip
            break;
        }

        if(chunk->numStructures < MAX_STRUCTURES)
        {
            chunk->structures[chunk->numStructures++] = (Structure) {.type = structureSpawnData->type,
                                                                        .isSource = true,
                                                                        .x = baseX,
                                                                        .y = groundHeight + 1,
                                                                        .z = baseZ};
        }
        //Max one structure per chunk
        break;
    }
    
    //Calculate basic AABB
    calcChunkAABB(chunk);

    //We've generated structure data
    CHUNK_SET_FLAG(chunk, CHUNK_NEW_STRUCT_DATA);
    //Mark chunk as initial - has been generated, no player-made modifications
    CHUNK_SET_FLAG(chunk, CHUNK_IS_INITIAL);
    //We know the chunk was only just created, so it has no draw data yet
    CHUNK_SET_FLAG(chunk, CHUNK_NO_DRAW_DATA);
    //Don't mark the chunk to be built yet - we're not done!
}

void propagateChunkStructureData(Chunk* sourceChunk, Chunk** adjacentChunks, uint8_t numAdjacentChunks)
{
    for(uint8_t i = 0; i < sourceChunk->numStructures; i++)
    {
        if(!sourceChunk->structures[i].isSource)
        {
            continue;
        }

        AABB structureAABB = getAABBForStructure(&sourceChunk->structures[i]);
        moveAABB(&structureAABB, sourceChunk->aabb.min);
        
        if(aabbInsideAABB(&structureAABB, &sourceChunk->aabb))
        {
            //Easy case: structure is entirely inside this chunk
            continue;
        }

        for(uint8_t j = 0; j < numAdjacentChunks; j++)
        {
            Chunk* adjacentChunk = adjacentChunks[j];

            //Skip the source chunk if it's in the list
            //Also skip any chunks that have been player-modified
            if(adjacentChunk == sourceChunk || !CHUNK_GET_FLAG(adjacentChunk, CHUNK_IS_INITIAL))
            {
                continue;
            }

            //Skip this chunk if no part of the structure is inside it
            if(!aabbIntersectsAABB(&structureAABB, &adjacentChunk->aabb))
            {
                continue;
            }

            bool equal = false;
            for(uint8_t k = 0; k < adjacentChunk->numStructures; k++)
            {
                if(isStructureDataEqual(&adjacentChunk->structures[k], &sourceChunk->structures[i]))
                {
                    equal = true;
                    break;
                }
            }

            if(equal)
            {
                continue;
            }
            
            if(adjacentChunk->numStructures < MAX_STRUCTURES)
            {
                int16_t xDiff = (sourceChunk->position.x - adjacentChunk->position.x) * CHUNK_SIZE;
                int16_t yDiff = (sourceChunk->position.y - adjacentChunk->position.y) * CHUNK_SIZE;
                int16_t zDiff = (sourceChunk->position.z - adjacentChunk->position.z) * CHUNK_SIZE;

                adjacentChunk->structures[adjacentChunk->numStructures++] = (Structure) {
                    .type = sourceChunk->structures[i].type,
                    .isSource = false,
                    .x = sourceChunk->structures[i].x + xDiff,
                    .y = sourceChunk->structures[i].y + yDiff,
                    .z = sourceChunk->structures[i].z + zDiff
                };

                CHUNK_SET_FLAG(adjacentChunk, CHUNK_NEW_STRUCT_DATA);
            }
            #ifdef DEBUG
            else
            {
                printf("Failed to propagate structure data into chunk (%d %d %d).\n", adjacentChunk->position.x, adjacentChunk->position.y, adjacentChunk->position.z);
            }
            #endif
        }
    }
}

void generateStructurePart(Chunk* chunk, Structure* structure)
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

    //Generate a "base plate" if necessary
    //This will not extend down past this single chunk, but hopefully that's good enough
    vec2u8 baseStart = {start.x < structureDef.basePos.x ? structureDef.basePos.x : start.x,
                        start.z < structureDef.basePos.z ? structureDef.basePos.z : start.z};
    vec2u8 baseEnd = {structureDef.basePos.x + structureDef.baseSize.x,
                        structureDef.basePos.z + structureDef.baseSize.z};
    if(structureDef.baseBlock.type != BLOCK_AIR)
    {
        for(uint8_t j = 0; j < offset.y; j++)
        {
            for(uint8_t i = baseStart.x; i < baseEnd.x; i++)
            {
                uint8_t x = offset.x + i - start.x;
                if(x >= CHUNK_SIZE)
                {
                    continue;
                }
                for(uint8_t k = baseStart.z; k < baseEnd.z; k++)
                {
                    uint8_t z = offset.z + k - start.z;
                    if(z >= CHUNK_SIZE)
                    {
                        continue;
                    }
                    CHUNK_BLOCK(chunk, x, j, z) = structureDef.baseBlock;
                }
            }
        }
    }

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
        generateStructurePart(chunk, &chunk->structures[i]);
    }

    //*Now* we're done with this chunk
    //All structures have been generated
    CHUNK_CLEAR_FLAG(chunk, CHUNK_NEW_STRUCT_DATA);
    //Mark chunk geometry to be built
    CHUNK_SET_FLAG(chunk, CHUNK_MODIFIED);
    //Mark chunk as initial - has been generated, no player-made modifications
    CHUNK_SET_FLAG(chunk, CHUNK_IS_INITIAL);
}
