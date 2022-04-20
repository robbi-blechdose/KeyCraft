#include "inventory.h"

BlockType hotbar[HOTBAR_SIZE] = HOTBAR_INITIAL;
uint8_t hotbarCursor = 0;

void drawHotbar()
{
    //TODO
}

void scrollHotbar()
{
    hotbarCursor++;
    if(hotbarCursor == HOTBAR_SIZE)
    {
        hotbarCursor = 0;
    }
}

uint8_t getHotbarSelection()
{
    return hotbar[hotbarCursor];
}