#include "inventory.h"

#include "engine/image.h"
#include "engine/video.h"
#include "engine/util.h"
#include "engine/savegame.h"

BlockType hotbar[HOTBAR_SIZE] = HOTBAR_INITIAL;
uint8_t hotbarCursor = 0;

BlockType inventory[INVENTORY_SIZE_X * INVENTORY_SIZE_Y] = INVENTORY_INITIAL;
uint8_t inventoryCursorX = 0;
uint8_t inventoryCursorY = 0;

#define UIBH 10

#define PTCL(X) PTC(X) + 0.001f
#define PTCH(X) PTC(X) - 0.001f

void drawBlockItem(BlockType type, float x, float y)
{
    vec2 blockTexture = getInventoryTextureForBlock(type);
    drawTexQuad(x, y, 24, 24, UIBH, PTCL(blockTexture.x), PTCL(blockTexture.y), PTCH(blockTexture.x + 8), PTCH(blockTexture.y + 8));
}

void drawSquare(float x, float y, uint8_t selected)
{
    if(selected)
    {
        drawTexQuad(x, y, 32, 32, UIBH, PTC(224), PTC(32), PTC(224 + 31), PTC(63));
    }
    else
    {
        drawTexQuad(x, y, 32, 32, UIBH, PTC(224), PTC(0), PTC(224 + 31), PTC(31));
    }
}

void drawHotbar()
{
    for(uint8_t i = 0; i < HOTBAR_SIZE; i++)
    {
        drawSquare(WINX / 2 - (32 * HOTBAR_SIZE / 2.0f) + (i * 32), 8, i == hotbarCursor);
        drawBlockItem(hotbar[i], WINX / 2 - (32 * HOTBAR_SIZE / 2.0f) + (i * 32) + 4, 8 + 4);
    }
    const char* text = blockNames[hotbar[hotbarCursor]];
    glDrawText(text, WINX / 2 - strlen(text) * 8 / 2, 240 - 52, 0xFFFFFF);
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

void drawInventory()
{
    for(uint8_t i = 0; i < INVENTORY_SIZE_X; i++)
    {
        for(uint8_t j = 0; j < INVENTORY_SIZE_Y; j++)
        {
            drawSquare(WINX / 2 - (32 * INVENTORY_SIZE_X / 2.0f) + (i * 32), 184 - (j * 32), i == inventoryCursorX && j == inventoryCursorY);
            if(inventory[i + j * INVENTORY_SIZE_X] == BLOCK_AIR)
            {
                continue;
            }
            drawBlockItem(inventory[i + j * INVENTORY_SIZE_X], WINX / 2 - (32 * INVENTORY_SIZE_X / 2.0f) + (i * 32) + 4, 184 - (j * 32) + 4);
        }
    }
}

void scrollInventory(int8_t dirX, int8_t dirY)
{
    inventoryCursorX += dirX;
    if(inventoryCursorX == INVENTORY_SIZE_X)
    {
        inventoryCursorX = 0;
    }
    else if(inventoryCursorX > INVENTORY_SIZE_X) //Overflow
    {
        inventoryCursorX = INVENTORY_SIZE_X - 1;
    }
    inventoryCursorY += dirY;
    if(inventoryCursorY == INVENTORY_SIZE_Y)
    {
        inventoryCursorY = 0;
    }
    else if(inventoryCursorY > INVENTORY_SIZE_Y) //Overflow
    {
        inventoryCursorY = INVENTORY_SIZE_Y - 1;
    }
}

void selectInventorySlot()
{
    hotbar[hotbarCursor] = inventory[inventoryCursorX + inventoryCursorY * INVENTORY_SIZE_X];
}

void saveHotbar()
{
    writeElement(&hotbarCursor, sizeof(uint8_t));
    for(uint8_t i = 0; i < HOTBAR_SIZE; i++)
    {
        writeElement(&hotbar[i], sizeof(BlockType));
    }
}

void loadHotbar()
{
    readElement(&hotbarCursor, sizeof(uint8_t));
    for(uint8_t i = 0; i < HOTBAR_SIZE; i++)
    {
        readElement(&hotbar[i], sizeof(BlockType));
    }
}