#ifndef _BLOCK_DRAWING_H
#define _BLOCK_DRAWING_H

#include "block.h"

vec2 getBlockTexture(BlockType type, uint8_t index);

void drawNormalBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);
void drawMultitexBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);
void drawXBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);
void drawDoor(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);
void drawFlatBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);

#endif