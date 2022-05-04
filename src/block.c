#include "block.h"

#include <GL/gl.h>

#include "engine/includes/3dMath.h"
#include "engine/image.h"

#define glVectorV3(vec) glVertex3f((vec).x, (vec).y, (vec).z)

/**
 * PTCL - Pointer to texture Low - "lower" corner
 * PTCH - Pointer to texture High - "higher" corner
 * Adds a small offset to prevent bleeding in the texture atlas
 **/
#define PTCL(X) PTC(X) + 0.001f
#define PTCH(X) PTC(X) - 0.001f

//Texture atlas
const vec2 textures[] = {
    { 0, 0}, //Bedrock
    { 8, 0}, //Stone
    {16, 0}, //Sand
    {24, 0}, //Dirt
    {32, 0}, //Grass side
    {40, 0}, //Grass top
    {48, 0}, //Wood side
    {56, 0}, //Wood top

    { 0, 8}, //Planks
    { 8, 8}, //Coal ore
    {16, 8}, //Iron ore
    {24, 8}, //Gold ore
    {32, 8}, //Redstone ore
    {40, 8}, //Diamond ore
    {48, 8}, //Red flower
    {56, 8}, //Yellow flower

    { 0, 16}, //Tall grass
    { 8, 16}, //Glass
    {16, 16}, //Leaves
    {24, 16}, //Bookshelf
    {32, 16}, //Wheat 0
    {40, 16}, //Wheat 1
    {48, 16}, //Wheat 2
    {56, 16}, //Wheat 3

    { 0, 24}, //Water
    { 8, 24}, //Door upper

    { 8, 32} //Door lower
};

const uint8_t normalBlockTextures[] = {
    [BLOCK_BEDROCK] = 0,
    [BLOCK_STONE] = 1,
    [BLOCK_SAND] = 2,
    [BLOCK_DIRT] = 3,
    [BLOCK_PLANKS] = 8,
    [BLOCK_COAL_ORE] = 9,
    [BLOCK_IRON_ORE] = 10,
    [BLOCK_GOLD_ORE] = 11,
    [BLOCK_REDSTONE_ORE] = 12,
    [BLOCK_DIAMOND_ORE] = 13,
    [BLOCK_GLASS] = 17,
    [BLOCK_LEAVES] = 18,
    [BLOCK_WATER] = 24
};

const uint8_t orientedBlockTextures[][6] = {
    [BLOCK_GRASS] = {4, 4, 4, 4, 5, 3},
    [BLOCK_WOOD] = {6, 6, 6, 6, 7, 7},
    [BLOCK_BOOKSHELF] = {19, 19, 19, 19, 8, 8}
};

const uint8_t xBlockTextures[] = {
    [BLOCK_FLOWER] = 14,
    [BLOCK_TALL_GRASS] = 16,
    [BLOCK_WHEAT] = 20
};

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
    [BLOCK_DOOR] = "Door"
};

void calcBlockVertices(vec3 list[8], float x, float y, float z, float xSize, float ySize, float zSize)
{
    list[0] = (vec3) {x, y, z + zSize};
    list[1] = (vec3) {x + xSize, y, z + zSize};
    list[2] = (vec3) {x + xSize, y + ySize, z + zSize};
    list[3] = (vec3) {x, y + ySize, z + zSize};

    list[4] = (vec3) {x + xSize, y, z};
    list[5] = (vec3) {x, y, z};
    list[6] = (vec3) {x, y + ySize, z};
    list[7] = (vec3) {x + xSize, y + ySize, z};
}

void calcBlockCorners(vec3 list[8], uint8_t x, uint8_t y, uint8_t z)
{
    calcBlockVertices(list, x, y, z, BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
}

void drawNormalBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    vec3 v[8];
    calcBlockCorners(v, x, y, z);

    vec3* faces[6][4] = {
        {&v[0], &v[1], &v[2], &v[3]},
        {&v[4], &v[5], &v[6], &v[7]},
        {&v[5], &v[0], &v[3], &v[6]},
        {&v[1], &v[4], &v[7], &v[2]},
        {&v[3], &v[2], &v[7], &v[6]},
        {&v[5], &v[4], &v[1], &v[0]}
    };

    //Get texture position
    vec2 tex = textures[normalBlockTextures[block->type]];
    float texX1 = PTCL(tex.x);
    float texX2 = PTCH(tex.x + 8);
    float texY1 = PTCL(tex.y);
    float texY2 = PTCH(tex.y + 8);

    uint8_t occlusionCheck = BS_FRONT;
    for(uint8_t i = 0; i < 6; i++)
    {
        if(occlusion & occlusionCheck)
        {
            occlusionCheck <<= 1;
            continue;
        }

        glTexCoord2f(texX2, texY2);
        glVectorV3(*(faces[i][0]));
        glTexCoord2f(texX1, texY2);
        glVectorV3(*(faces[i][1]));
        glTexCoord2f(texX1, texY1);
        glVectorV3(*(faces[i][2]));
        glTexCoord2f(texX2, texY1);
        glVectorV3(*(faces[i][3]));

        occlusionCheck <<= 1;
    }
}

//TODO: Rename
void drawOrientedBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    vec3 v[8];
    calcBlockCorners(v, x, y, z);

    vec3* faces[6][4] = {
        {&v[0], &v[1], &v[2], &v[3]},
        {&v[4], &v[5], &v[6], &v[7]},
        {&v[5], &v[0], &v[3], &v[6]},
        {&v[1], &v[4], &v[7], &v[2]},
        {&v[3], &v[2], &v[7], &v[6]},
        {&v[5], &v[4], &v[1], &v[0]}
    };

    //Get texture positions
    vec2 tex[6];
    for(uint8_t i = 0; i < 6; i++)
    {
        tex[i] = textures[orientedBlockTextures[block->type][i]];
    }

    uint8_t occlusionCheck = BS_FRONT;
    for(uint8_t i = 0; i < 6; i++)
    {
        if(occlusion & occlusionCheck)
        {
            occlusionCheck <<= 1;
            continue;
        }

        glTexCoord2f(PTCH(tex[i].x + 8), PTCH(tex[i].y + 8));
        glVectorV3(*(faces[i][0]));
        glTexCoord2f(PTCL(tex[i].x), PTCH(tex[i].y + 8));
        glVectorV3(*(faces[i][1]));
        glTexCoord2f(PTCL(tex[i].x), PTCL(tex[i].y));
        glVectorV3(*(faces[i][2]));
        glTexCoord2f(PTCH(tex[i].x + 8), PTCL(tex[i].y));
        glVectorV3(*(faces[i][3]));

        occlusionCheck <<= 1;
    }
}

void drawXBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    vec3 v[8];
    calcBlockCorners(v, x, y, z);

    vec3* faces[6][4] = {
        {&v[0], &v[4], &v[7], &v[3]},
        {&v[5], &v[1], &v[2], &v[6]},
    };

    //Get texture position
    vec2 tex = textures[xBlockTextures[block->type] + (block->data & BLOCK_DATA_TEXTURE)];
    float texX1 = PTCL(tex.x);
    float texX2 = PTCH(tex.x + 8);
    float texY1 = PTCL(tex.y);
    float texY2 = PTCH(tex.y + 8);

    glDisable(GL_CULL_FACE);
    for(uint8_t i = 0; i < 2; i++)
    {
        glTexCoord2f(texX2, texY2);
        glVectorV3(*(faces[i][0]));
        glTexCoord2f(texX1, texY2);
        glVectorV3(*(faces[i][1]));
        glTexCoord2f(texX1, texY1);
        glVectorV3(*(faces[i][2]));
        glTexCoord2f(texX2, texY1);
        glVectorV3(*(faces[i][3]));
    }
    glEnable(GL_CULL_FACE);
}

void drawDoor(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    //Calculate vertices
    uint8_t front = (block->data & BLOCK_DATA_DIRECTION) == BLOCK_DATA_DIR_FRONT;
    uint8_t back = (block->data & BLOCK_DATA_DIRECTION) == BLOCK_DATA_DIR_BACK;
    uint8_t left = (block->data & BLOCK_DATA_DIRECTION) == BLOCK_DATA_DIR_LEFT;
    uint8_t right = (block->data & BLOCK_DATA_DIRECTION) == BLOCK_DATA_DIR_RIGHT;
    float x1 = (front || !back) ? x : 1 - DOOR_WIDTH;
    float xS = (front || back) ? DOOR_WIDTH : BLOCK_SIZE;
    float z1 = (left || !right) ? z : 1 - DOOR_WIDTH;
    float zS = (left || right) ? DOOR_WIDTH : BLOCK_SIZE;

    vec3 v[8];
    calcBlockVertices(v, x1, y, z1, xS, BLOCK_SIZE, zS);

    vec3* faces[6][4] = {
        {&v[0], &v[1], &v[2], &v[3]},
        {&v[4], &v[5], &v[6], &v[7]},
        {&v[5], &v[0], &v[3], &v[6]},
        {&v[1], &v[4], &v[7], &v[2]},
        {&v[3], &v[2], &v[7], &v[6]},
        {&v[5], &v[4], &v[1], &v[0]}
    };

    //Door texture positions
    vec2 tex[6]; //TODO

    uint8_t occlusionCheck = BS_FRONT;
    for(uint8_t i = 0; i < 6; i++)
    {
        if(occlusion & occlusionCheck)
        {
            occlusionCheck <<= 1;
            continue;
        }

        glTexCoord2f(PTCH(tex[i].x + 8), PTCH(tex[i].y + 8));
        glVectorV3(*(faces[i][0]));
        glTexCoord2f(PTCL(tex[i].x), PTCH(tex[i].y + 8));
        glVectorV3(*(faces[i][1]));
        glTexCoord2f(PTCL(tex[i].x), PTCL(tex[i].y));
        glVectorV3(*(faces[i][2]));
        glTexCoord2f(PTCH(tex[i].x + 8), PTCL(tex[i].y));
        glVectorV3(*(faces[i][3]));

        occlusionCheck <<= 1;
    }
}

void drawBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    switch(block->type)
    {
        case BLOCK_GRASS:
        case BLOCK_WOOD:
        case BLOCK_BOOKSHELF:
        {
            drawOrientedBlock(block, x, y, z, occlusion);
            break;
        }
        case BLOCK_FLOWER:
        case BLOCK_TALL_GRASS:
        case BLOCK_WHEAT:
        {
            drawXBlock(block, x, y, z, occlusion);
            break;
        }
        case BLOCK_DOOR:
        {
            drawDoor(block, x, y, z, occlusion);
            break;
        }
        default:
        {
            drawNormalBlock(block, x, y, z, occlusion);
            break;
        }
    }
}

vec2 getTextureForBlock(BlockType block)
{
    switch(block)
    {
        case BLOCK_GRASS:
        case BLOCK_WOOD:
        case BLOCK_BOOKSHELF:
        {
            return textures[orientedBlockTextures[block][0]];
        }
        case BLOCK_FLOWER:
        case BLOCK_TALL_GRASS:
        case BLOCK_WHEAT:
        {
            return textures[xBlockTextures[block]];
        }
        default:
        {
            return textures[normalBlockTextures[block]];
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
        case BLOCK_DOOR:
        {
            //TODO: Check block above
            return (below != BLOCK_AIR);
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
        {
            return 0;
        }
        default:
        {
            return 1;
        }
    }
}