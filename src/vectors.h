#ifndef VECTORS_H
#define VECTORS_H

#include <stdint.h>

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t z;
} vec3u8;

typedef struct {
    uint8_t x;
    uint8_t z;
} vec2u8;

#endif