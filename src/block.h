#ifndef _BLOCK_H
#define _BLOCK_H

#include <stdint.h>

typedef enum {
    BLOCK_AIR = 0,
    BLOCK_STONE,
    BLOCK_DIRT,
    BLOCK_GRASS,
    BLOCK_PLANKS,
    //TODO
    BLOCK_BRICKS,
    //TODO
    BLOCK_FLOWER,
    BLOCK_MUSHROOM,
    BLOCK_TALL_GRASS,
    //TODO
    BLOCK_COBBLESTONE,
    BLOCK_BEDROCK,
    BLOCK_SAND,
    //TODO
    BLOCK_WOOD,
    //TODO
    BLOCK_GOLD_ORE,
    BLOCK_IRON_ORE,
    BLOCK_COAL_ORE,
    BLOCK_DIAMOND_ORE,
    BLOCK_REDSTONE_ORE,
    //TODO
    BLOCK_CRAFTING_TABLE,
    BLOCK_FURNACE,
    //TODO
    BLOCK_DOOR,
    BLOCK_REDSTONE_TORCH,
    BLOCK_REDSTONE_WIRE,
    BLOCK_REDSTONE_REPEATER,
    //TODO
    BLOCK_LAMP,
    //TODO
    BLOCK_WATER,
    BLOCK_LAVA,
    //TODO
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
 * XXXX XXPP
 * P = Power state
 **/

/** ---------- Block data definitions ---------- **/
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
uint8_t isFullBlock(BlockType type);

#endif