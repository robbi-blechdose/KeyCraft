#ifndef INVENTORY_H
#define INVENTORY_H

#include "blocks/block.h"

void drawHotbar();

void scrollHotbar();
Block getHotbarSelection();

void drawInventory();

void scrollInventory(int8_t dirX, int8_t dirY, int8_t dirTab);
void selectInventorySlot();

void saveHotbar();
void loadHotbar();

void resetHotbar();

#endif