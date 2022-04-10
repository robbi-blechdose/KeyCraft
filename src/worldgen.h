#ifndef _WORLDGEN_H
#define _WORLDGEN_H

#include <stdint.h>

#include "chunk.h"

void initWorldgen();
void generateChunk(Chunk* chunk, int16_t x, int16_t y, int16_t z);

#endif