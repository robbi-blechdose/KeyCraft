#include "blockdrawing.h"

#include <GL/gl.h>

#include "../engine/includes/3dMath.h"
#include "../engine/image.h"

#define glVectorV3(vec) glVertex3f((vec).x, (vec).y, (vec).z)

/**
 * PTCL - Pointer to texture Low - "lower" corner
 * PTCH - Pointer to texture High - "higher" corner
 * Adds a small offset to prevent bleeding in the texture atlas
 **/
#define PTCL(X) PTC(X) + 0.001f
#define PTCH(X) PTC(X) - 0.001f

//Texture atlas
//Texture order: front, rear, left, right, up, down

typedef struct {
    uint8_t x;
    uint8_t y;
} vec2i;

const vec2i* blockTextures[] = {
    [BLOCK_BEDROCK] =    (vec2i[1]) {{0, 0}},
    [BLOCK_STONE] =      (vec2i[1]) {{1, 0}},
    [BLOCK_SAND] =       (vec2i[1]) {{2, 0}},
    [BLOCK_DIRT] =       (vec2i[1]) {{3, 0}},
    [BLOCK_GRASS] =      (vec2i[6]) {{4, 0}, {4, 0}, {4, 0}, {4, 0}, {5, 0}, {3, 0}},
    [BLOCK_WOOD] =       (vec2i[6]) {{6, 0}, {6, 0}, {6, 0}, {6, 0}, {7, 0}, {7, 0}},

    [BLOCK_PLANKS] =     (vec2i[1]) {{0, 1}},
    [BLOCK_COAL_ORE] =   (vec2i[1]) {{1, 1}},
    [BLOCK_IRON_ORE] =   (vec2i[1]) {{2, 1}},
    [BLOCK_GOLD_ORE] =   (vec2i[1]) {{3, 1}},
    [BLOCK_REDSTONE_ORE] = (vec2i[1]) {{4, 1}},
    [BLOCK_DIAMOND_ORE] = (vec2i[1]) {{5, 1}},
    [BLOCK_FLOWER] =     (vec2i[2]) {{6, 1}, {7, 1}},

    [BLOCK_TALL_GRASS] = (vec2i[1]) {{0, 2}},
    [BLOCK_GLASS] =      (vec2i[1]) {{1, 2}},
    [BLOCK_LEAVES] =     (vec2i[1]) {{2, 2}},
    [BLOCK_BOOKSHELF] =  (vec2i[6]) {{3, 2}, {3, 2}, {3, 2}, {3, 2}, {0, 1}, {0, 1}},
    [BLOCK_WHEAT] =      (vec2i[4]) {{4, 2}, {5, 2}, {6, 2}, {7, 2}},

    [BLOCK_WATER] =      (vec2i[1]) {{0, 3}},
    [BLOCK_DOOR] =       (vec2i[3]) {{0, 1}, {1, 3}, {1, 4}},
    [BLOCK_REDSTONE_LAMP] = (vec2i[2]) {{2, 3}, {2, 4}},
    [BLOCK_REDSTONE_WIRE] = (vec2i[2]) {{4, 3}, {4, 4}},
    [BLOCK_REDSTONE_TORCH] = (vec2i[2]) {{5, 3}, {5, 4}},
    [BLOCK_COBBLESTONE] = (vec2i[1]) {{6, 3}},

    [BLOCK_SUGAR_CANE] = (vec2i[1]) {{0, 4}},
    [BLOCK_TNT] = (vec2i[6]) {{6, 4}, {6, 4}, {6, 4}, {6, 4}, {6, 5}, {6, 5}},
    [BLOCK_CRAFTING_TABLE] = (vec2i[6]) {{1, 5}, {1, 5}, {2, 5}, {2, 5}, {0, 5}, {0, 1}},

    [BLOCK_FURNACE] = (vec2i[6]) {{0, 6}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}}
};

vec2 getBlockTexture(BlockType type, uint8_t index)
{
    return (vec2) {blockTextures[type][index].x * 8.0f, blockTextures[type][index].y * 8.0f};
}

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
    vec2 tex = getBlockTexture(block->type, block->data & BLOCK_DATA_TEXTURE);
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

void drawMultitexBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
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
        tex[i] = getBlockTexture(block->type, i);
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
    vec2 tex = getBlockTexture(block->type, block->data & BLOCK_DATA_TEXTURE);
    float texX1 = PTCL(tex.x);
    float texX2 = PTCH(tex.x + 8);
    float texY1 = PTCL(tex.y);
    float texY2 = PTCH(tex.y + 8);

    //Don't do backface culling (that way, we only have to draw 2 faces instead of 4 with possible z-fighting when viewing from the side)
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
    float x1 = (front || !back) ? x : x + 1 - DOOR_WIDTH;
    float xS = (front || back) ? DOOR_WIDTH : BLOCK_SIZE;
    float z1 = (left || !right) ? z : z + 1 - DOOR_WIDTH;
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
    vec2 texFB0 = getBlockTexture(block->type, block->data & BLOCK_DATA_PART ? 1 : 2);
    vec2 texFB1 = {texFB0.x + 8, texFB0.y + 8};
    vec2 texLR0 = getBlockTexture(block->type, 0);
    vec2 texLR1 = {texLR0.x + 1, texLR0.y + 8};

    uint8_t occlusionCheck = BS_FRONT;
    for(uint8_t i = 0; i < 6; i++)
    {
        if(occlusion & occlusionCheck)
        {
            occlusionCheck <<= 1;
            //continue;
        }

        vec2 tex0 = texLR0;
        vec2 tex1 = texLR1;
        if(((front || back) && (i == 2 || i == 3)) ||
            ((left || right) && (i == 0 || i == 1)))
        {
            tex0 = texFB0;
            tex1 = texFB1;
        }

        glTexCoord2f(PTCH(tex1.x), PTCH(tex1.y));
        glVectorV3(*(faces[i][0]));
        glTexCoord2f(PTCL(tex0.x), PTCH(tex1.y));
        glVectorV3(*(faces[i][1]));
        glTexCoord2f(PTCL(tex0.x), PTCL(tex0.y));
        glVectorV3(*(faces[i][2]));
        glTexCoord2f(PTCH(tex1.x), PTCL(tex0.y));
        glVectorV3(*(faces[i][3]));

        occlusionCheck <<= 1;
    }
}

void drawFlatBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    //Get texture position
    //TODO: "connected textures"?
    vec2 tex = getBlockTexture(block->type, block->data & BLOCK_DATA_TEXTURE);
    float texX1 = PTCL(tex.x);
    float texX2 = PTCH(tex.x + 8);
    float texY1 = PTCL(tex.y);
    float texY2 = PTCH(tex.y + 8);
    
    //Draw face
    //Uses direct coordinates since it's only a single face
    glTexCoord2f(texX2, texY2);
    glVertex3f(x, y + (BLOCK_SIZE / 20), z + BLOCK_SIZE);
    glTexCoord2f(texX1, texY2);
    glVertex3f(x + BLOCK_SIZE, y + (BLOCK_SIZE / 20), z + BLOCK_SIZE);
    glTexCoord2f(texX1, texY1);
    glVertex3f(x + BLOCK_SIZE, y + (BLOCK_SIZE / 20), z);
    glTexCoord2f(texX2, texY1);
    glVertex3f(x, y + (BLOCK_SIZE / 20), z);
}

#define BP (BLOCK_SIZE / 8.0f)

//TODO: Rotation
void drawSwitch(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    //Base
    vec3 v[8];
    calcBlockVertices(v, x + BP, y, z + BP * 2, BP * 6, BP * 2, BP * 4);

    vec3* faces[5][4] = {
        {&v[4], &v[5], &v[6], &v[7]},
        {&v[5], &v[0], &v[3], &v[6]},
        {&v[1], &v[4], &v[7], &v[2]},
        {&v[3], &v[2], &v[7], &v[6]},
        {&v[5], &v[4], &v[1], &v[0]}
    };

    vec2 tex = getBlockTexture(BLOCK_COBBLESTONE, 0);
    float texX1 = PTCL(tex.x);
    float texY1 = PTCL(tex.y);
    vec2 faceTextures[5] = {
        {PTCH(tex.x + 6), PTCH(tex.y + 4)},
        {PTCH(tex.x + 6), PTCH(tex.y + 4)},
        {PTCH(tex.x + 6), PTCH(tex.y + 4)},
        {PTCH(tex.x + 6), PTCH(tex.y + 4)},
        {PTCH(tex.x + 6), PTCH(tex.y + 4)},
    };

    for(uint8_t i = 0; i < 5; i++)
    {
        glTexCoord2f(faceTextures[i].x, faceTextures[i].y);
        glVectorV3(*(faces[i][0]));
        glTexCoord2f(texX1, faceTextures[i].y);
        glVectorV3(*(faces[i][1]));
        glTexCoord2f(texX1, texY1);
        glVectorV3(*(faces[i][2]));
        glTexCoord2f(faceTextures[i].x, texY1);
        glVectorV3(*(faces[i][3]));
    }

    //Lever

    //TODO
}