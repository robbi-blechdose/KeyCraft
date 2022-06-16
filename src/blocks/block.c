#include "block.h"

#include "blockdrawing.h"

const char* blockNames[] = {
    [BLOCK_STONE] = "Stone",
    [BLOCK_SAND] = "Sand",
    [BLOCK_DIRT] = "Dirt",
    [BLOCK_GRASS] = "Grass",
    [BLOCK_WOOD] = "Wood",

    [BLOCK_PLANKS] = "Wood planks",
    [BLOCK_COAL_ORE] = "Coal ore",
    [BLOCK_IRON_ORE] = "Iron ore",
    [BLOCK_GOLD_ORE] = "Gold ore",
    [BLOCK_REDSTONE_ORE] = "Redstone ore",
    [BLOCK_DIAMOND_ORE] = "Diamond ore",
    [BLOCK_FLOWER] = "Flower",

    [BLOCK_TALL_GRASS] = "Tall grass",
    [BLOCK_GLASS] = "Glass",
    [BLOCK_LEAVES] = "Leaves",
    [BLOCK_BOOKSHELF] = "Bookshelf",
    [BLOCK_WHEAT] = "Wheat",

    [BLOCK_WATER] = "Water",
    [BLOCK_DOOR] = "Door",
    [BLOCK_REDSTONE_LAMP] = "Redstone lamp",
    [BLOCK_REDSTONE_WIRE] = "Redstone wire",
    [BLOCK_REDSTONE_TORCH] = "Redstone torch",
    [BLOCK_COBBLESTONE] = "Cobblestone",

    [BLOCK_SUGAR_CANE] = "Sugar cane",
    [BLOCK_CRAFTING_TABLE] = "Crafting table",

    [BLOCK_FURNACE] = "Furnace"
};

void drawBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    switch(block->type)
    {
        case BLOCK_GRASS:
        case BLOCK_WOOD:
        case BLOCK_BOOKSHELF:
        case BLOCK_CRAFTING_TABLE:
        case BLOCK_FURNACE:
        {
            drawMultitexBlock(block, x, y, z, occlusion);
            break;
        }
        case BLOCK_FLOWER:
        case BLOCK_TALL_GRASS:
        case BLOCK_WHEAT:
        case BLOCK_REDSTONE_TORCH:
        case BLOCK_SUGAR_CANE:
        {
            drawXBlock(block, x, y, z, occlusion);
            break;
        }
        case BLOCK_DOOR:
        {
            drawDoor(block, x, y, z, occlusion);
            break;
        }
        case BLOCK_REDSTONE_WIRE:
        {
            drawFlatBlock(block, x, y, z, occlusion);
            break;
        }
        default:
        {
            drawNormalBlock(block, x, y, z, occlusion);
            break;
        }
    }
}

vec2 getInventoryTextureForBlock(Block block)
{
    switch(block.type)
    {
        case BLOCK_DOOR:
        {
            return getBlockTexture(block.type, 1);
        }
        case BLOCK_WHEAT:
        {
            return getBlockTexture(block.type, 3);
        }
        default:
        {
            return getBlockTexture(block.type, block.data & BLOCK_DATA_TEXTURE);
        }
    }
}

uint8_t isOpaqueBlock(BlockType type)
{
    switch(type)
    {
        case BLOCK_AIR:
        case BLOCK_FLOWER:
        case BLOCK_TALL_GRASS:
        case BLOCK_GLASS:
        case BLOCK_WHEAT:
        case BLOCK_DOOR:
        case BLOCK_REDSTONE_TORCH:
        case BLOCK_REDSTONE_WIRE:
        case BLOCK_SUGAR_CANE:
        {
            return 0;
        }
        default:
        {
            return 1;
        }
    }
}

uint8_t canPlaceBlock(BlockType toPlace, BlockType below)
{
    switch(toPlace)
    {
        case BLOCK_WHEAT:
        case BLOCK_FLOWER:
        case BLOCK_TALL_GRASS:
        {
            return (below == BLOCK_DIRT || below == BLOCK_GRASS);
        }
        case BLOCK_SUGAR_CANE:
        {
            return (below == BLOCK_SAND || below == BLOCK_SUGAR_CANE);
        }
        case BLOCK_DOOR:
        case BLOCK_REDSTONE_TORCH:
        case BLOCK_REDSTONE_WIRE:
        {
            return isBlockCollidable(below);
        }
        default:
        {
            return 1;
        }
    }
}

uint8_t isBlockCollidable(BlockType type)
{
    switch(type)
    {
        case BLOCK_AIR:
        case BLOCK_WHEAT:
        case BLOCK_FLOWER:
        case BLOCK_TALL_GRASS:
        case BLOCK_WATER:
        case BLOCK_REDSTONE_TORCH:
        case BLOCK_REDSTONE_WIRE:
        case BLOCK_SUGAR_CANE:
        {
            return 0;
        }
        default:
        {
            return 1;
        }
    }
}

uint8_t isBlockOriented(BlockType type)
{
    switch(type)
    {
        case BLOCK_DOOR:
        {
            return 1;
        }
        default:
        {
            return 0;
        }
    }
}