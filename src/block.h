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
    BLOCK_BOOKSHELF,
    BLOCK_WHEAT,

    BLOCK_WATER,

    BLOCK_REDSTONE_LAMP,
    BLOCK_REDSTONE_WIRE,
    BLOCK_REDSTONE_TORCH,

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
 * XXXX XXXX
 *        TT - Texture, if set block uses normal texture + offset. Used by: Flower, Wheat, Water
 *      CC   - Counter. Used by: Wheat, Water
 *       DDD - Direction, determines which way the block is facing.
 **/

/** ---------- Block data definitions ---------- **/
#define BLOCK_DATA_TEXTURE  0b00000011
#define BLOCK_DATA_TEXTURE1 0b00000001
#define BLOCK_DATA_TEXTURE2 0b00000010
#define BLOCK_DATA_TEXTURE3 0b00000011

#define BLOCK_DATA_COUNTER  0b00001100
#define BLOCK_DATA_COUNTER1 0b00000100

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
uint8_t canPlaceBlock(BlockType toPlace, BlockType below);
uint8_t isBlockCollidable(BlockType type);

#endif