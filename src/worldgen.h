#ifndef _WORLDGEN_H
#define _WORLDGEN_H

#include <stdint.h>

#include "chunk.h"

void initWorldgen(uint32_t seed);
void generateChunk(Chunk* chunk);

#endif