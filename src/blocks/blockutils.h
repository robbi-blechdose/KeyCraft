#ifndef _BLOCK_UTILS_H
#define _BLOCK_UTILS_H

#include <stdint.h>

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} ChunkPos;

typedef struct {
    ChunkPos chunk;
    int8_t x;
    int8_t y;
    int8_t z;
} BlockPos;

void getBlockPosByDirection(uint8_t dir, BlockPos* pos);

/**
 * Differences from the center position to the adjacent blocks:
 *  3
 * 0X1
 *  2
 */
extern int8_t adjacentDiffs[4][2];

#endif