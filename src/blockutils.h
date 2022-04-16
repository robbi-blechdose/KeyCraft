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
    uint8_t x;
    uint8_t y;
    uint8_t z;
} BlockPos;

#endif