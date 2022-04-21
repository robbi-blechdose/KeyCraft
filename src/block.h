#ifndef _BLOCK_H
#define _BLOCK_H

#include <stdint.h>

#define BLOCK_SIZE 1.0f

typedef enum {
    BLOCK_AIR = 0,
    BLOCK_BEDROCK,
    BLOCK_STONE,
    BLOCK_SAND,
    BLOCK_DIRT,
    BLOCK_GRASS,
    BLOCK_WOOD,

    BLOCK_PLANKS,
    BLOCK_COAL_ORE,
    BLOCK_IRON_ORE,
    BLOCK_GOLD_ORE,
    BLOCK_REDSTONE_ORE,
    BLOCK_DIAMOND_ORE,
    BLOCK_FLOWER,

    BLOCK_TALL_GRASS,
    BLOCK_GLASS,
    BLOCK_LEAVES,

    BLOCK_LAST
} BlockType;

#define NUM_BLOCK_TYPES BLOCK_LAST

typedef struct {
    uint8_t type;
    uint8_t data;
} Block;

//TODO
/**
 * Block data has the following format:
 * XXXX XXXT
 * T = Type, if set block uses a different texture (normal texture + 1). Ignored if block has no secondary type
 **/

/** ---------- Block data definitions ---------- **/
#define BLOCK_DATA_TYPE 0b00000001
//TODO
/** -------------------------------------------- **/

typedef enum {
    BS_FRONT  = 0b00000001,
    BS_BACK   = 0b00000010,
    BS_LEFT   = 0b00000100,
    BS_RIGHT  = 0b00001000,
    BS_TOP    = 0b00010000,
    BS_BOTTOM = 0b00100000,
    BS_ALL    = 0b00111111
} BlockSide;

void drawBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);
uint8_t isOpaqueBlock(BlockType type);

#endif