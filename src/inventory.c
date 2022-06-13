#include "inventory.h"

#include "engine/image.h"
#include "engine/video.h"
#include "engine/util.h"
#include "engine/savegame.h"

#define HOTBAR_SIZE 3

#define NUM_INVENTORY_TABS 3
#define INVENTORY_SIZE_X 6
#define INVENTORY_SIZE_Y 4

#define B(X) {(X), 0}

Block hotbar[HOTBAR_SIZE] = {B(BLOCK_PLANKS), B(BLOCK_WOOD), B(BLOCK_COBBLESTONE)};
uint8_t hotbarCursor = 0;

typedef struct {
    char* name;
    uint8_t size;
    Block blocks[INVENTORY_SIZE_X * INVENTORY_SIZE_Y];
} InventoryTab;


const InventoryTab inventory[NUM_INVENTORY_TABS] = {
    {
        "Basic", 19,
        {
            B(BLOCK_STONE), B(BLOCK_SAND), B(BLOCK_DIRT), B(BLOCK_GRASS), B(BLOCK_WOOD), B(BLOCK_PLANKS),
            B(BLOCK_COAL_ORE), B(BLOCK_IRON_ORE), B(BLOCK_GOLD_ORE), B(BLOCK_REDSTONE_ORE), B(BLOCK_DIAMOND_ORE),
            B(BLOCK_GLASS), B(BLOCK_LEAVES), B(BLOCK_BOOKSHELF), B(BLOCK_WATER),
            B(BLOCK_DOOR), B(BLOCK_COBBLESTONE), B(BLOCK_CRAFTING_TABLE), B(BLOCK_FURNACE)
        }
    },
    {
        "Plants", 4,
        {
            B(BLOCK_FLOWER), {BLOCK_FLOWER, BLOCK_DATA_TEXTURE1}, B(BLOCK_TALL_GRASS), B(BLOCK_WHEAT)
        }
    },
    {
        "Redstone", 3,
        {
            B(BLOCK_REDSTONE_LAMP), B(BLOCK_REDSTONE_WIRE), {BLOCK_REDSTONE_TORCH, BLOCK_DATA_POWER}
        }
    }
};

uint8_t inventoryTab = 0;
uint8_t inventoryCursorX = 0;
uint8_t inventoryCursorY = 0;

#define UIBH 10

#define PTCL(X) PTC(X) + 0.001f
#define PTCH(X) PTC(X) - 0.001f

void drawBlockItem(Block block, float x, float y)
{
    vec2 blockTexture = getInventoryTextureForBlock(block);
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
    const char* text = blockNames[hotbar[hotbarCursor].type];
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

Block getHotbarSelection()
{
    return hotbar[hotbarCursor];
}

void drawInventory()
{
    glDrawText(inventory[inventoryTab].name, WINX / 2 - strlen(inventory[inventoryTab].name) * 8 / 2, 16, 0xFFFFFF);

    for(uint8_t i = 0; i < INVENTORY_SIZE_X; i++)
    {
        for(uint8_t j = 0; j < INVENTORY_SIZE_Y; j++)
        {
            drawSquare(WINX / 2 - (32 * INVENTORY_SIZE_X / 2.0f) + (i * 32), 168 - (j * 32), i == inventoryCursorX && j == inventoryCursorY);
            if(i + j * INVENTORY_SIZE_X >= inventory[inventoryTab].size)
            {
                continue;
            }
            drawBlockItem(inventory[inventoryTab].blocks[i + j * INVENTORY_SIZE_X], WINX / 2 - (32 * INVENTORY_SIZE_X / 2.0f) + (i * 32) + 4, 168 - (j * 32) + 4);
        }
    }
}

void scrollInventory(int8_t dirX, int8_t dirY, int8_t dirTab)
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
    inventoryTab += dirTab;
    if(inventoryTab == NUM_INVENTORY_TABS)
    {
        inventoryTab = 0;
    }
    else if(inventoryTab > NUM_INVENTORY_TABS) //Overflow
    {
        inventoryTab = NUM_INVENTORY_TABS - 1;
    }
}

void selectInventorySlot()
{
    if(inventoryCursorX + inventoryCursorY * INVENTORY_SIZE_X < inventory[inventoryTab].size)
    {
        hotbar[hotbarCursor] = inventory[inventoryTab].blocks[inventoryCursorX + inventoryCursorY * INVENTORY_SIZE_X];
    }
}

void saveHotbar()
{
    writeElement(&hotbarCursor, sizeof(uint8_t));
    for(uint8_t i = 0; i < HOTBAR_SIZE; i++)
    {
        writeElement(&hotbar[i], sizeof(Block));
    }
}

void loadHotbar()
{
    readElement(&hotbarCursor, sizeof(uint8_t));
    for(uint8_t i = 0; i < HOTBAR_SIZE; i++)
    {
        readElement(&hotbar[i], sizeof(Block));
    }
}