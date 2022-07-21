#ifndef _BLOCK_H
#define _BLOCK_H

#include <stdbool.h>
#include <stdint.h>

#include "../engine/util.h"

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
    BLOCK_DOOR,
    BLOCK_REDSTONE_LAMP,
    BLOCK_REDSTONE_WIRE,
    BLOCK_REDSTONE_TORCH,
    BLOCK_REDSTONE_REPEATER,
    BLOCK_TNT,

    BLOCK_SUGAR_CANE,
    BLOCK_CRAFTING_TABLE,
    BLOCK_COBBLESTONE,
    BLOCK_PISTON,
    BLOCK_PISTON_BASE,
    BLOCK_PISTON_HEAD,

    BLOCK_FURNACE,
    BLOCK_CACTUS,
    BLOCK_DEAD_SHRUB,
    BLOCK_COMPUTER,

    BLOCK_LEVER,

    BLOCK_LAST
} BlockType;

#define NUM_BLOCK_TYPES BLOCK_LAST

typedef struct {
    uint8_t type;
    uint8_t data;
} Block;

/**
 * Block data has the following format:
 * XXXX XXXX
 *        TT - Texture, if set block uses normal texture + offset. Used by: Flower, Wheat, Water
 *      CC   - Counter. Used by: Wheat, Water
 * DD        - Direction, determines which way the block is facing. Used by: Door, piston
 *         P - Part, determines which part of a "multiblock" this is. Used by: Door
 *        S  - State. Used by: Door
 *      R    - Redstone power. Used by: Redstone wire, torch, lever
 *       V   - Visited, used for block traversal logic. Used by: Redstone wire
 **/

/** ---------- Block data definitions ---------- **/
#define BLOCK_DATA_TEXTURE   0b00000011
#define BLOCK_DATA_TEXTURE1  0b00000001
#define BLOCK_DATA_TEXTURE2  0b00000010
#define BLOCK_DATA_TEXTURE3  0b00000011

#define BLOCK_DATA_COUNTER   0b00001100
#define BLOCK_DATA_COUNTER1  0b00000100

#define BLOCK_DATA_DIRECTION 0b11000000
#define BLOCK_DATA_DIR_FRONT 0b00000000
#define BLOCK_DATA_DIR_BACK  0b01000000
#define BLOCK_DATA_DIR_LEFT  0b10000000
#define BLOCK_DATA_DIR_RIGHT 0b11000000

#define BLOCK_DATA_PART      0b00000001

#define BLOCK_DATA_STATE     0b00000010

#define BLOCK_DATA_POWER     0b00001000

#define BLOCK_DATA_VISITED   0b00000100

#define BLOCK_DATA_COMPUTER  0b00001111

/** -------------------------------------------- **/

#define BLOCK_SIZE 1.0f
#define BLOCK_PIXEL(X) ((BLOCK_SIZE / 8) * (X))
#define DOOR_WIDTH (BLOCK_SIZE / 8)

typedef enum {
    BS_FRONT  = 0b00000001,
    BS_BACK   = 0b00000010,
    BS_LEFT   = 0b00000100,
    BS_RIGHT  = 0b00001000,
    BS_TOP    = 0b00010000,
    BS_BOTTOM = 0b00100000,
    BS_ALL    = 0b00111111
} BlockSide;

extern const char* blockNames[];

void drawBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);

vec2 getInventoryTextureForBlock(Block block);

bool isOpaqueBlock(BlockType type);
bool canPlaceBlock(BlockType toPlace, BlockType below);
bool isBlockCollidable(BlockType type);
bool isBlockOriented(BlockType type);

#endif