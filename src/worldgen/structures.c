#include "structures.h"

#define B(X) {(X), 0}
//Shorthand for air block
#define BA B(BLOCK_AIR)
//Structure definitions have their own shorthand

StructureDefinition structureDefinitions[NUM_STRUCTURE_TYPES] = {

#define BW B(BLOCK_WOOD)
#define BL B(BLOCK_LEAVES)

    [STRUCTURE_TREE] = {
        .size = {5, 6, 5},
        .blocks = (Block[]) {
            //0th slice
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BW, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA,
            //1st slice
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BW, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA,
            //2nd slice
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BW, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA,
            //3rd slice (leaves start)
            BA, BL, BL, BL, BA,
            BL, BL, BL, BL, BL,
            BL, BL, BW, BL, BL,
            BL, BL, BL, BL, BL,
            BA, BL, BL, BL, BA,
            //4th slice
            BA, BL, BL, BL, BA,
            BL, BL, BL, BL, BL,
            BL, BL, BW, BL, BL,
            BL, BL, BL, BL, BL,
            BA, BL, BL, BL, BA,
            //5th slice (trunk end)
            BA, BA, BA, BA, BA,
            BA, BL, BL, BL, BA,
            BA, BL, BL, BL, BA,
            BA, BL, BL, BL, BA,
            BA, BA, BA, BA, BA
        },
        .spawnOnBlockType = BLOCK_GRASS,
        .spawnOnBlockPosX = 2,
        .spawnOnBlockPosZ = 2,
    },
    [STRUCTURE_TREE_TALL] = {
        .size = {5, 11, 5},
        .blocks = (Block[]) {
            //0th slice
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BW, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA,
            //1st slice
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BW, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA,
            //2nd slice
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BW, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA,
            //3rd slice (leaves start)
            BA, BL, BL, BL, BA,
            BL, BL, BL, BL, BL,
            BL, BL, BW, BL, BL,
            BL, BL, BL, BL, BL,
            BA, BL, BL, BL, BA,
            //4th slice
            BA, BL, BL, BL, BA,
            BL, BL, BL, BL, BL,
            BL, BL, BW, BL, BL,
            BL, BL, BL, BL, BL,
            BA, BL, BL, BL, BA,
            //5th slice
            BA, BL, BL, BL, BA,
            BL, BL, BL, BL, BL,
            BL, BL, BW, BL, BL,
            BL, BL, BL, BL, BL,
            BA, BL, BL, BL, BA,
            //6th slice (smaller leaves)
            BA, BA, BA, BA, BA,
            BA, BL, BL, BL, BA,
            BA, BL, BL, BL, BA,
            BA, BL, BL, BL, BA,
            BA, BA, BA, BA, BA,
            //7th slice
            BA, BA, BA, BA, BA,
            BA, BL, BL, BL, BA,
            BA, BL, BL, BL, BA,
            BA, BL, BL, BL, BA,
            BA, BA, BA, BA, BA,
            //8th slice
            BA, BA, BA, BA, BA,
            BA, BL, BL, BL, BA,
            BA, BL, BL, BL, BA,
            BA, BL, BL, BL, BA,
            BA, BA, BA, BA, BA,
            //9th slice (thin top leaves)
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BL, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA,
            //10th slice
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BL, BA, BA,
            BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA
        },
        .spawnOnBlockType = BLOCK_GRASS,
        .spawnOnBlockPosX = 2,
        .spawnOnBlockPosZ = 2,
    },

#define BB B(BLOCK_BASALT)
#undef BL
#define BL B(BLOCK_LAVA)

    [STRUCTURE_VOLCANO] = {
        .size = {7, 6, 7},
        .blocks = (Block[]) {
            //0th slice
            BB, BB, BB, BB, BB, BB, BB,
            BB, BB, BB, BB, BB, BB, BB,
            BB, BB, BB, BB, BB, BB, BB,
            BB, BB, BB, BL, BB, BB, BB,
            BB, BB, BB, BB, BB, BB, BB,
            BB, BB, BB, BB, BB, BB, BB,
            BB, BB, BB, BB, BB, BB, BB,
            //1st slice (smaller)
            BA, BA, BA, BA, BA, BA, BA,
            BA, BB, BB, BB, BB, BB, BA,
            BA, BB, BB, BB, BB, BB, BA,
            BA, BB, BB, BL, BB, BB, BA,
            BA, BB, BB, BB, BB, BB, BA,
            BA, BB, BB, BB, BB, BB, BA,
            BA, BA, BA, BA, BA, BA, BL,
            //2nd slice
            BA, BA, BA, BA, BA, BA, BA,
            BA, BA, BB, BB, BB, BA, BA,
            BA, BB, BB, BB, BB, BB, BA,
            BA, BB, BB, BL, BB, BB, BA,
            BA, BB, BB, BB, BB, BB, BA,
            BA, BA, BB, BB, BB, BL, BA,
            BA, BA, BA, BA, BA, BA, BA,
            //3nd slice (smaller)
            BA, BA, BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA, BA, BA,
            BA, BA, BB, BB, BB, BA, BA,
            BA, BA, BB, BL, BB, BA, BA,
            BA, BA, BB, BB, BB, BA, BA,
            BA, BA, BA, BA, BA, BL, BA,
            BA, BA, BA, BA, BA, BA, BA,
            //4th slice
            BA, BA, BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA, BA, BA,
            BA, BA, BB, BB, BB, BA, BA,
            BA, BA, BB, BL, BB, BA, BA,
            BA, BA, BB, BB, BB, BA, BA,
            BA, BA, BA, BA, BA, BL, BA,
            BA, BA, BA, BA, BA, BA, BA,
            //5th slice
            BA, BA, BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA, BA, BA,
            BA, BA, BA, BB, BA, BA, BA,
            BA, BA, BB, BL, BB, BA, BA,
            BA, BA, BA, BB, BL, BA, BA,
            BA, BA, BA, BA, BA, BA, BA,
            BA, BA, BA, BA, BA, BA, BA
        },
        .spawnOnBlockType = BLOCK_AIR
    },

#define BC B(BLOCK_COBBLESTONE)

    [STRUCTURE_ROCK_PILE] = {
        .size = {2, 2, 2},
        .blocks = (Block[]) {
            //0th slice
            BC, BC,
            BC, BC,
            //1st slice
            BC, BC,
            BC, BA
        },
        .spawnOnBlockType = BLOCK_AIR
    }
};

AABB getAABBForStructure(StructureData* structure)
{
    AABB aabb = {
        .min = (vec3) {0, 0, 0},
        .max = {structureDefinitions[structure->type].size.x,
                structureDefinitions[structure->type].size.y,
                structureDefinitions[structure->type].size.z}
    };
    moveAABB(&aabb, (vec3) {structure->x, structure->y, structure->z});
    return aabb;
}

bool isStructureDataEqual(StructureData* a, StructureData* b)
{
    return a->isSource == b->isSource &&
            a->type == b->type &&
            a->x == b->x &&
            a->y == b->y &&
            a->z == b->z;
}