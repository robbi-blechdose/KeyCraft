#ifndef _INVENTORY_H
#define _INVENTORY_H

#include "block.h"

#define HOTBAR_SIZE 3
#define HOTBAR_INITIAL {BLOCK_PLANKS, BLOCK_GLASS, BLOCK_SAND}

void drawHotbar();

void scrollHotbar();
uint8_t getHotbarSelection();

#endif