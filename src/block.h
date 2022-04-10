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
    BLOCK_FLOWER,
    //TODO
    BLOCK_BEDROCK,
    //TODO
    BLOCK_LAST
} BlockType;

#define NUM_BLOCK_TYPES BLOCK_LAST

typedef struct {
    uint8_t type;
} Block;

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