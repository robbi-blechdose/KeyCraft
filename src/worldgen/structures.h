#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdint.h>

#include "../aabb.h"
#include "../blocks/block.h"

/**
 * Structure system: Allows generating structures larger than a single chunk without requiring all adjacent chunks to be loaded
 * Phases:
 * - chunk terrain generation: happens as normal
 * - structure pre-generation: generate structure data (per chunk)
 * - structure data propagation: create structure data for adjacent chunks (that are set to be generated) from the ones that already have data
 * - structure generation: generate structure blocks (per chunk)
 */

typedef enum {
    STRUCTURE_TREE,
    STRUCTURE_TREE_TALL,
    STRUCTURE_TREE_WIDE,
    STRUCTURE_VOLCANO,
    STRUCTURE_ROCK_PILE,
    NUM_STRUCTURE_TYPES
} StructureType;

//TODO: possibly move somewhere better?
typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t z;
} vec3u8;

typedef struct {
    vec3u8 size;
    //Block data is laid out in X-Z "slices" (with each slice being made up of Z rows in the X direction), the 0th slice being the bottom one
    Block* blocks;
    //If this is set to a non-BLOCK_AIR, this block will be generated as a "base plate" to ensure the structure isn't flying
    Block baseBlock;
    //TODO: specify size for base (instead of whole structure size)
    //Spawn prerequisites
    //If the block type is set to BLOCK_AIR, the structure may spawn anywhere
    BlockType spawnOnBlockType;
    uint8_t spawnOnBlockPosX;
    uint8_t spawnOnBlockPosZ;
} StructureDefinition;

//TODO: rename: this is an actual placed structure in a chunk (maybe something like "placedStructure?")
typedef struct {
    StructureType type;
    //The position stores the origin of the structure - with {0, 0, 0} being the origin of the current chunk
    int8_t x;
    int8_t y;
    int8_t z;
    bool isSource;
} StructureData;

extern StructureDefinition structureDefinitions[NUM_STRUCTURE_TYPES];

AABB getAABBForStructure(StructureData* structure);
bool isStructureDataEqual(StructureData* a, StructureData* b);

#endif