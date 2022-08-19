#ifndef _BLOCK_DRAWING_H
#define _BLOCK_DRAWING_H

#include "block.h"

vec2 getBlockTexture(BlockType type, uint8_t index);

void drawNormalBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);
void drawSlab(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);

void drawMultitexBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);
void drawMultitexBlockWithRotation(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);

void drawXBlock(Block* block, uint8_t x, uint8_t y, uint8_t z);

void drawDoor(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);

void drawFlatBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);
void drawFlatBlockWithRotation(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);

void drawSwitch(Block* block, uint8_t x, uint8_t y, uint8_t z);

void drawPistonBase(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);
void drawPistonHead(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion);

#endif