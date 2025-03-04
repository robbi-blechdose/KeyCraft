#ifndef BIOMES_H
#define BIOMES_H

#include "structures.h"

typedef enum {
    BIOME_NORMAL,
    BIOME_DESERT,
    BIOME_BARREN,
    BIOME_ROCKS,
    BIOME_FOREST,
    NUM_BIOME_TYPES
} BiomeType;

struct Interval {
    float start;
    float end;
};

struct StructureSpawnData {
    StructureType type;
    //The "random" noise in worldgen is a 3d noise to allow different elements to have different noise values. This determines the Y coordinate
    uint8_t randomNoiseY;
    //If the random noise value is < than this chance, the structure is spawned
    float spawnChance;
};

typedef struct {
    //If the biome random noise value is inside this interval, this biome is chosen
    struct Interval spawnChance;
    //Determines which structures spawn inside this biome
    uint8_t numStructureSpawnData;
    struct StructureSpawnData* structureSpawnData;
} BiomeDefinition;

extern BiomeDefinition biomeDefinitions[NUM_BIOME_TYPES];

#endif