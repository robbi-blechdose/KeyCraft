#ifndef _INVENTORY_H
#define _INVENTORY_H

#include "block.h"

#define HOTBAR_SIZE 3
#define HOTBAR_INITIAL {BLOCK_PLANKS, BLOCK_GLASS, BLOCK_WHEAT}

#define INVENTORY_SIZE_X 6
#define INVENTORY_SIZE_Y 4
#define INVENTORY_INITIAL { \
    BLOCK_STONE, BLOCK_SAND, BLOCK_DIRT, BLOCK_GRASS, BLOCK_WOOD, BLOCK_PLANKS, \
    BLOCK_COAL_ORE, BLOCK_IRON_ORE, BLOCK_GOLD_ORE, BLOCK_REDSTONE_ORE, BLOCK_DIAMOND_ORE, BLOCK_FLOWER, \
    BLOCK_TALL_GRASS, BLOCK_GLASS, BLOCK_LEAVES, BLOCK_BOOKSHELF, BLOCK_WHEAT, BLOCK_WATER, \
    BLOCK_DOOR \
}

void drawHotbar();

void scrollHotbar();
uint8_t getHotbarSelection();

void drawInventory();

void scrollInventory(int8_t dirX, int8_t dirY);
void selectInventorySlot();

#endif