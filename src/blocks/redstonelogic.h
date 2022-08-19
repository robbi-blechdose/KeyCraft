#ifndef _REDSTONE_LOGIC_H
#define _REDSTONE_LOGIC_H

#include <stdbool.h>
#include <stdint.h>

#include "block.h"
#include "../chunk.h"

/**
 * Checks if the block at the given position has powered blocks next to it
 * @param onlySource Limits check to direct power sources (levers, torches)
 **/
bool hasAdjacentPower(ChunkPos chunk, uint8_t x, uint8_t y, uint8_t z, bool onlySource);

void tickRedstoneWire(Chunk* chunk, Block* block, uint8_t x, uint8_t y, uint8_t z);

void tickRedstoneTorch(Chunk* chunk, Block* block, uint8_t x, uint8_t y, uint8_t z);

void tickRedstoneRepeater(Chunk* chunk, Block* block, uint8_t x, uint8_t y, uint8_t z);

#endif