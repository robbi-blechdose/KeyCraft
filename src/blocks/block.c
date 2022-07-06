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
    //[BLOCK_REDSTONE_REPEATER] = "Redstone repeater",
    [BLOCK_TNT] = "TNT",

    [BLOCK_SUGAR_CANE] = "Sugar cane",
    [BLOCK_CRAFTING_TABLE] = "Crafting table",
    [BLOCK_COBBLESTONE] = "Cobblestone",
    [BLOCK_PISTON] = "Piston",

    [BLOCK_FURNACE] = "Furnace",

    [BLOCK_LEVER] = "Lever"
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
        case BLOCK_TNT:
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
            drawXBlock(block, x, y, z);
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
        case BLOCK_LEVER:
        {
            drawSwitch(block, x, y, z);
            break;
        }
        case BLOCK_PISTON:
        {
            //TODO: Rotation, correct texture orientation
            drawMultitexBlock(block, x, y, z, occlusion);
            break;
        }
        case BLOCK_PISTON_BASE:
        {
            drawPistonBase(block, x, y, z, occlusion);
            break;
        }
        case BLOCK_PISTON_HEAD:
        {
            drawPistonHead(block, x, y, z, occlusion);
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
        case BLOCK_LEVER:
        {
            return (vec2) {64, 0};
        }
        case BLOCK_PISTON:
        {
            return (vec2) {72, 0};
        }
        default:
        {
            return getBlockTexture(block.type, block.data & BLOCK_DATA_TEXTURE);
        }
    }
}

bool isOpaqueBlock(BlockType type)
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
        case BLOCK_LEVER:
        case BLOCK_PISTON_BASE:
        case BLOCK_PISTON_HEAD:
        {
            return false;
        }
        default:
        {
            return true;
        }
    }
}

bool canPlaceBlock(BlockType toPlace, BlockType below)
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
        case BLOCK_LEVER:
        {
            return isBlockCollidable(below);
        }
        default:
        {
            return true;
        }
    }
}

bool isBlockCollidable(BlockType type)
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
        case BLOCK_LEVER:
        {
            return false;
        }
        default:
        {
            return true;
        }
    }
}

bool isBlockOriented(BlockType type)
{
    switch(type)
    {
        case BLOCK_DOOR:
        case BLOCK_LEVER:
        case BLOCK_PISTON:
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
}