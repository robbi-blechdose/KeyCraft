#include "inventory.h"

#include "engine/image.h"
#include "engine/video.h"

BlockType hotbar[HOTBAR_SIZE] = HOTBAR_INITIAL;
uint8_t hotbarCursor = 0;

#define UIBH 10

void drawHotbar()
{
    for(uint8_t i = 0; i < HOTBAR_SIZE; i++)
    {
        if(i == hotbarCursor)
        {
            drawTexQuad(WINX / 2 - (32 * 1.5f) + (i * 32), 8, 32, 32, UIBH, PTC(72), PTC(32), PTC(72 + 31), PTC(63));
        }
        else
        {
            drawTexQuad(WINX / 2 - (32 * 1.5f) + (i * 32), 8, 32, 32, UIBH, PTC(72), PTC(0), PTC(72 + 31), PTC(31));
        }
    }
    //TODO: Draw blocks
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