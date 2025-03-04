#ifndef WORLDGEN_H
#define WORLDGEN_H

#include <stdint.h>

#include "../chunk.h"

void initWorldgen(uint32_t seed);

void generateChunk(Chunk* chunk);
void propagateChunkStructureData(Chunk* sourceChunk, Chunk** adjacentChunks, uint8_t numAdjacentChunks);
void generateChunkStructures(Chunk* chunk);

#endif