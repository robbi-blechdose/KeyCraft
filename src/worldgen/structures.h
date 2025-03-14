#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdint.h>

#include "../aabb.h"
#include "../blocks/block.h"
#include "../vectors.h"

/**
 * Structure system: Allows generating structures larger than a single chunk without requiring all adjacent chunks to be loaded
 * Phases:
 * - chunk terrain generation: happens as normal
 * - structure pre-generation: generate structure data (per chunk)
 * - structure data propagation: create structure data for adjacent chunks (that are set to be generated) from the ones that already have data
 * - structure generation: generate structure blocks (per chunk)
 */

typedef enum {
    STRUCTURE_SHRUB,
    STRUCTURE_TREE,
    STRUCTURE_TREE_WIDE,
    STRUCTURE_TREE_FLAT,
    STRUCTURE_TREE_TALL_DARK,
    STRUCTURE_VOLCANO,
    STRUCTURE_ROCK_PILE,
    NUM_STRUCTURE_TYPES
} StructureType;

typedef struct {
    vec3u8 size;
    //Block data is laid out in X-Z "slices" (with each slice being made up of Z rows in the X direction), the 0th slice being the bottom one
    Block* blocks;
    //If this is set to a non-BLOCK_AIR, this block will be generated as a "base plate" to ensure the structure isn't flying
    Block baseBlock;
    vec2u8 basePos;
    vec2u8 baseSize;
    //Spawn prerequisites
    //If the block type is set to BLOCK_AIR, the structure may spawn anywhere
    BlockType spawnOnBlockType;
    uint8_t spawnOnBlockPosX;
    uint8_t spawnOnBlockPosZ;
} StructureDefinition;

typedef struct {
    StructureType type;
    //The position stores the origin of the structure - with {0, 0, 0} being the origin of the current chunk
    int8_t x;
    int8_t y;
    int8_t z;
    bool isSource;
} Structure;

extern StructureDefinition structureDefinitions[NUM_STRUCTURE_TYPES];

AABB getAABBForStructure(Structure* structure);
bool isStructureDataEqual(Structure* a, Structure* b);

#endif