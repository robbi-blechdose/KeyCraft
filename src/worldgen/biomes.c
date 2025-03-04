#include "biomes.h"

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
        }
    },
    [BIOME_DESERT] = {
        .spawnChance = {.start = 0.0f, .end = 0.2f},
        .numStructureSpawnData = 0
    },
    [BIOME_BARREN] = {
        .spawnChance = {.start = 0.8f, .end = 1.0f},
        .numStructureSpawnData = 1,
        .structureSpawnData = (struct StructureSpawnData[]) {
            {
                .type = STRUCTURE_ROCK_PILE,
                .randomNoiseY = 24,
                .spawnChance = 0.5f
            }
        }
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
        }
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
        }
    }
};