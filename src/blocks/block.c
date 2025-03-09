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
    [BLOCK_REDSTONE_REPEATER] = "Redstone repeater",
    [BLOCK_TNT] = "TNT",

    [BLOCK_SUGAR_CANE] = "Sugar cane",
    [BLOCK_CRAFTING_TABLE] = "Crafting table",
    [BLOCK_COBBLESTONE] = "Cobblestone",
    [BLOCK_PISTON] = "Piston",

    [BLOCK_FURNACE] = "Furnace",
    [BLOCK_CACTUS] = "Cactus",
    [BLOCK_DEAD_SHRUB] = "Dead shrub",
    [BLOCK_COMPUTER] = "Computer",

    [BLOCK_BRICKS] = "Bricks",
    [BLOCK_MUSHROOM] = "Mushroom",

    [BLOCK_LEVER] = "Lever",

    [BLOCK_WOOD_SLAB] = "Wood slab",
    [BLOCK_COBBLESTONE_SLAB] = "Cobblestone slab",

    [BLOCK_NOTEBLOCK] = "Note block",
    [BLOCK_LAVA] = "Lava",
    [BLOCK_MOSS] = "Moss",
    [BLOCK_BASALT] = "Basalt",

    [BLOCK_PRESSURE_PLATE] = "Pressure plate"
};

//TODO: pass texture index into functions here already
//This allows us to use more than the 2 bits of texture, e.g. for a repeater (3 positions x2 for on/off) or the note block (8 positions)
void drawBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    switch(block->type)
    {
        case BLOCK_GRASS:
        case BLOCK_WOOD:
        case BLOCK_BOOKSHELF:
        case BLOCK_CRAFTING_TABLE:
        case BLOCK_TNT:
        case BLOCK_CACTUS:
        {
            drawMultitexBlock(block, x, y, z, occlusion);
            break;
        }
        case BLOCK_FLOWER:
        case BLOCK_TALL_GRASS:
        case BLOCK_WHEAT:
        case BLOCK_REDSTONE_TORCH:
        case BLOCK_SUGAR_CANE:
        case BLOCK_DEAD_SHRUB:
        case BLOCK_MUSHROOM:
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
        case BLOCK_MOSS:
        case BLOCK_PRESSURE_PLATE:
        {
            drawFlatBlock(block, x, y, z, occlusion);
            break;
        }
        case BLOCK_REDSTONE_REPEATER:
        {
            drawFlatBlockWithRotation(block, x, y, z, occlusion);
            break;
        }
        case BLOCK_LEVER:
        {
            drawSwitch(block, x, y, z);
            break;
        }
        case BLOCK_PISTON:
        case BLOCK_COMPUTER:
        case BLOCK_FURNACE:
        {
            drawMultitexBlockWithRotation(block, x, y, z, occlusion);
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
        case BLOCK_WOOD_SLAB:
        case BLOCK_COBBLESTONE_SLAB:
        {
            drawSlab(block, x, y, z, occlusion);
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
        case BLOCK_WOOD:
        {
            return getBlockTexture(block.type, (block.data & BLOCK_DATA_TEXTURE) * 6);   
        }
        case BLOCK_LEVER:
        {
            return (vec2) {64, 0};
        }
        case BLOCK_PISTON:
        {
            return (vec2) {72, 0};
        }
        case BLOCK_WOOD_SLAB:
        {
            return (vec2) {80, 0};
        }
        case BLOCK_COBBLESTONE_SLAB:
        {
            return (vec2) {88, 0};
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
        case BLOCK_REDSTONE_REPEATER:
        case BLOCK_DEAD_SHRUB:
        case BLOCK_MUSHROOM:
        case BLOCK_WOOD_SLAB:
        case BLOCK_COBBLESTONE_SLAB:
        case BLOCK_MOSS:
        case BLOCK_PRESSURE_PLATE:
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
        case BLOCK_MUSHROOM:
        {
            return (below == BLOCK_DIRT || below == BLOCK_GRASS);
        }
        case BLOCK_SUGAR_CANE:
        {
            return (below == BLOCK_SAND || below == BLOCK_SUGAR_CANE);
        }
        case BLOCK_CACTUS:
        {
            return (below == BLOCK_SAND || below == BLOCK_CACTUS);
        }
        case BLOCK_DEAD_SHRUB:
        {
            return (below == BLOCK_DIRT || below == BLOCK_GRASS || below == BLOCK_SAND);
        }
        case BLOCK_DOOR:
        case BLOCK_REDSTONE_TORCH:
        case BLOCK_REDSTONE_WIRE:
        case BLOCK_LEVER:
        case BLOCK_REDSTONE_REPEATER:
        case BLOCK_MOSS:
        case BLOCK_PRESSURE_PLATE:
        {
            return isBlockCollidable(below) && !(below == BLOCK_WOOD_SLAB || below == BLOCK_COBBLESTONE_SLAB || below == BLOCK_PRESSURE_PLATE);
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
        case BLOCK_REDSTONE_REPEATER:
        case BLOCK_DEAD_SHRUB:
        case BLOCK_MUSHROOM:
        case BLOCK_LAVA:
        case BLOCK_MOSS:
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
        case BLOCK_REDSTONE_REPEATER:
        case BLOCK_COMPUTER:
        case BLOCK_FURNACE:
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
}