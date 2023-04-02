#include "blockdrawing.h"

#include <GL/gl.h>

#include "../engine/includes/3dMath.h"
#include "../engine/image.h"

#define glVectorV3(vec) glVertex3f((vec).x, (vec).y, (vec).z)

#define BLOCK_ALL_FACES(V)  {\
        {&V[0], &V[1], &V[2], &V[3]},\
        {&V[4], &V[5], &V[6], &V[7]},\
        {&V[5], &V[0], &V[3], &V[6]},\
        {&V[1], &V[4], &V[7], &V[2]},\
        {&V[3], &V[2], &V[7], &V[6]},\
        {&V[5], &V[4], &V[1], &V[0]}\
    }

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

const vec2i* const blockTextures[] = {
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
    [BLOCK_FLOWER] =     (vec2i[4]) {{6, 1}, {7, 1}, {3, 7}, {4, 7}},

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
    [BLOCK_REDSTONE_REPEATER] = (vec2i[2]) {{6, 3}, {6, 4}},
    [BLOCK_TNT] = (vec2i[6]) {{7, 3}, {7, 3}, {7, 3}, {7, 3}, {7, 4}, {7, 4}},

    [BLOCK_SUGAR_CANE] = (vec2i[1]) {{0, 4}},
    [BLOCK_CRAFTING_TABLE] = (vec2i[6]) {{1, 5}, {1, 5}, {2, 5}, {2, 5}, {0, 5}, {0, 1}},
    [BLOCK_COBBLESTONE] = (vec2i[1]) {{3, 5}},
    [BLOCK_PISTON] = (vec2i[6]) {{6, 5}, {7, 5}, {5, 5}, {5, 5}, {5, 5}, {5, 5}},
    [BLOCK_PISTON_BASE] = (vec2i[3]) {{4, 5}, {5, 5}, {7, 5}},
    [BLOCK_PISTON_HEAD] = (vec2i[2]) {{6, 5}, {0, 1}},

    [BLOCK_FURNACE] = (vec2i[7]) {{0, 6}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {7, 5}, {1, 6}},
    [BLOCK_CACTUS] = (vec2i[6]) {{2, 6}, {2, 6}, {2, 6}, {2, 6}, {3, 6}, {3, 6}},
    [BLOCK_DEAD_SHRUB] = (vec2i[1]) {{4, 6}},
    [BLOCK_COMPUTER] = (vec2i[6]) {{5, 6}, {6, 6}, {7, 6}, {7, 6}, {7, 6}, {7, 6}},

    [BLOCK_BRICKS] = (vec2i[1]) {{0, 7}},
    [BLOCK_MUSHROOM] = (vec2i[2]) {{1, 7}, {2, 7}},

    [BLOCK_WOOD_SLAB] = (vec2i[1]) {{0, 1}},
    [BLOCK_COBBLESTONE_SLAB] = (vec2i[1]) {{3, 5}},

    [BLOCK_NOTEBLOCK] = (vec2i[1]) {{5, 7}}
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

void drawNormalBlockHeight(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion, uint8_t height)
{
    vec3 v[8];
    calcBlockVertices(v, x, y, z, BLOCK_SIZE, BLOCK_PIXEL(height), BLOCK_SIZE);
    vec3* faces[6][4] = BLOCK_ALL_FACES(v);

    //Get texture position
    vec2 tex = getBlockTexture(block->type, block->data & BLOCK_DATA_TEXTURE);
    float texX1 = PTCL(tex.x);
    float texX2 = PTCH(tex.x + 8);
    float texY1 = PTCL(tex.y);
    float texY2 = PTCH(tex.y + height);

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

void drawNormalBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    drawNormalBlockHeight(block, x, y, z, occlusion, 8);
}

void drawSlab(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    //Exclude occlusion for the top of the slab (we still have to draw it if there's a block above)
    occlusion &= ~(BS_TOP);
    drawNormalBlockHeight(block, x, y, z, occlusion, 4);
}

void drawMultitexBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    vec3 v[8];
    calcBlockCorners(v, x, y, z);
    vec3* faces[6][4] = BLOCK_ALL_FACES(v);

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

void swapTexture(vec2 textures[6], uint8_t a, uint8_t b)
{
    vec2 temp;
    temp = textures[a];
    textures[a] = textures[b];
    textures[b] = temp;
}

void drawMultitexBlockWithRotation(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    uint8_t dir = block->data & BLOCK_DATA_DIRECTION;
    uint8_t occlusionCheck[6] = {BS_FRONT, BS_BACK, BS_LEFT, BS_RIGHT, BS_TOP, BS_BOTTOM};

    glEnd();

    glPushMatrix();
    glTranslatef(x + 0.5f, y, z + 0.5f);
    if(dir == BLOCK_DATA_DIR_BACK)
    {
        glRotatef(180, 0, 1, 0);
        //Switch occlusion checks
        const uint8_t newChecks[4] = {BS_BACK, BS_FRONT, BS_RIGHT, BS_LEFT};
        memcpy(occlusionCheck, newChecks, sizeof(newChecks));
    }
    else if(dir == BLOCK_DATA_DIR_RIGHT)
    {
        glRotatef(90, 0, 1, 0);
        //Switch occlusion checks
        const uint8_t newChecks[4] = {BS_RIGHT, BS_LEFT, BS_FRONT, BS_BACK};
        memcpy(occlusionCheck, newChecks, sizeof(newChecks));
    }
    else if(dir == BLOCK_DATA_DIR_LEFT)
    {
        glRotatef(270, 0, 1, 0);
        //Switch occlusion checks
        const uint8_t newChecks[4] = {BS_LEFT, BS_RIGHT, BS_BACK, BS_FRONT};
        memcpy(occlusionCheck, newChecks, sizeof(newChecks));
    }
    //Front is default (0 deg rotation)

    glBegin(GL_QUADS);

    //Calculate vertices
    vec3 v[8];
    calcBlockVertices(v, -0.5f, 0, -0.5f, BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
    vec3* faces[6][4] = BLOCK_ALL_FACES(v);
    //Rotate the one incorrect face
    faces[1][0] = &v[6];
    faces[1][1] = &v[7];
    faces[1][2] = &v[4];
    faces[1][3] = &v[5];

    //Get texture positions
    vec2 tex[6];
    for(uint8_t i = 0; i < 6; i++)
    {
        tex[i] = getBlockTexture(block->type, i);
        if(i == 0 && block->data & BLOCK_DATA_TEXTURE)
        {
            tex[i] = getBlockTexture(block->type, 5 + (block->data & BLOCK_DATA_TEXTURE));
        }
    }

    //Swap b+l
    swapTexture(tex, 0, 2);
    //Swap f+r
    swapTexture(tex, 1, 3);

    for(uint8_t i = 0; i < 6; i++)
    {
        if(occlusion & occlusionCheck[i])
        {
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
    }

    glEnd();

    glPopMatrix();

    glBegin(GL_QUADS);
}

void drawXBlock(Block* block, uint8_t x, uint8_t y, uint8_t z)
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

void swapTextures(vec2 textures[6][2], uint8_t a, uint8_t b)
{
    vec2 temp[2];
    temp[0] = textures[a][0];
    temp[1] = textures[a][1];
    textures[a][0] = textures[b][0];
    textures[a][1] = textures[b][1];
    textures[b][0] = temp[0];
    textures[b][1] = temp[1];
}

void drawPartBlock(uint8_t dir, uint8_t x, uint8_t y, uint8_t z, float size, bool startFront, vec2 textures[6][2])
{
    //Calculate vertices
    uint8_t front = dir == BLOCK_DATA_DIR_FRONT;
    uint8_t back = dir == BLOCK_DATA_DIR_BACK;
    uint8_t left = dir == BLOCK_DATA_DIR_LEFT;
    uint8_t right = dir == BLOCK_DATA_DIR_RIGHT;
    float x1;
    float xS;
    float z1;
    float zS;
    if(startFront)
    {
        x1 = (front || !back) ? x : x + 1 - size;
        xS = (front || back) ? size : BLOCK_SIZE;
        z1 = (left || !right) ? z : z + 1 - size;
        zS = (left || right) ? size : BLOCK_SIZE;
    }
    else
    {
        x1 = (!front || back) ? x : x + 1 - size;
        xS = (front || back) ? size : BLOCK_SIZE;
        z1 = (!left || right) ? z : z + 1 - size;
        zS = (left || right) ? size : BLOCK_SIZE;
    }

    vec3 v[8];
    calcBlockVertices(v, x1, y, z1, xS, BLOCK_SIZE, zS);
    vec3* faces[6][4] = BLOCK_ALL_FACES(v);

    //Do nothing for front (standard texture order)
    if(back)
    {
        //Swap f+b
        swapTextures(textures, 0, 1);
        //Swap l+r
        swapTextures(textures, 2, 3);
    }
    else if(left)
    {
        //Swap b+r
        swapTextures(textures, 0, 3);
        //Swap f+l
        swapTextures(textures, 1, 2);
    }
    else if(right)
    {
        //Swap b+l
        swapTextures(textures, 0, 2);
        //Swap f+r
        swapTextures(textures, 1, 3);
    }

    for(uint8_t i = 0; i < 6; i++)
    {
        vec2 tex0 = textures[i][0];
        vec2 tex1 = textures[i][1];

        //"rotate" texture by 90° if we're at the top or bottom
        if(i > 3 && (left || right))
        {
            glTexCoord2f(PTCH(tex1.x), PTCL(tex0.y));
            glVectorV3(*(faces[i][0]));
            glTexCoord2f(PTCH(tex1.x), PTCH(tex1.y));
            glVectorV3(*(faces[i][1]));
            glTexCoord2f(PTCL(tex0.x), PTCH(tex1.y));
            glVectorV3(*(faces[i][2]));
            glTexCoord2f(PTCL(tex0.x), PTCL(tex0.y));
            glVectorV3(*(faces[i][3]));
        }
        else
        {
            glTexCoord2f(PTCH(tex1.x), PTCH(tex1.y));
            glVectorV3(*(faces[i][0]));
            glTexCoord2f(PTCL(tex0.x), PTCH(tex1.y));
            glVectorV3(*(faces[i][1]));
            glTexCoord2f(PTCL(tex0.x), PTCL(tex0.y));
            glVectorV3(*(faces[i][2]));
            glTexCoord2f(PTCH(tex1.x), PTCL(tex0.y));
            glVectorV3(*(faces[i][3]));
        }
    }
}

void drawDoor(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    vec2 texFB0 = getBlockTexture(block->type, block->data & BLOCK_DATA_PART ? 1 : 2);
    vec2 texFB1 = {texFB0.x + 8, texFB0.y + 8};
    vec2 texLR0 = getBlockTexture(block->type, 0);
    vec2 texLR1 = {texLR0.x + 1, texLR0.y + 8};

    vec2 textures[6][2] = {
        texLR0, texLR1,
        texLR0, texLR1,
        texFB0, texFB1,
        texFB0, texFB1,
        texLR0, texLR1,
        texLR0, texLR1
    };

    drawPartBlock(block->data & BLOCK_DATA_DIRECTION, x, y, z, DOOR_WIDTH, true, textures);
}

void drawFlatBlockInternal(Block* block, float x, uint8_t y, float z, uint8_t occlusion)
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

void drawFlatBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    drawFlatBlockInternal(block, x, y, z, occlusion);
}

void drawFlatBlockWithRotation(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    uint8_t dir = block->data & BLOCK_DATA_DIRECTION;

    glEnd();
    glPushMatrix();
    glTranslatef(x + 0.5f, y, z + 0.5f);

    if(dir == BLOCK_DATA_DIR_BACK)
    {
        glRotatef(180, 0, 1, 0);
    }
    else if(dir == BLOCK_DATA_DIR_RIGHT)
    {
        glRotatef(90, 0, 1, 0);
    }
    else if(dir == BLOCK_DATA_DIR_LEFT)
    {
        glRotatef(270, 0, 1, 0);
    }
    //Front is default (0 deg rotation)

    glBegin(GL_QUADS);

    drawFlatBlockInternal(block, -0.5f, 0, -0.5f, occlusion);

    glEnd();
    glPopMatrix();
    glBegin(GL_QUADS);
}

void drawSwitch(Block* block, uint8_t x, uint8_t y, uint8_t z)
{
    //5 faces (bottom isn't needed)
    vec3 v[8];
    vec3* faces[5][4] = {
        {&v[0], &v[1], &v[2], &v[3]},
        {&v[4], &v[5], &v[6], &v[7]},
        {&v[5], &v[0], &v[3], &v[6]},
        {&v[1], &v[4], &v[7], &v[2]},
        {&v[3], &v[2], &v[7], &v[6]}
    };

    //Base
    if((block->data & BLOCK_DATA_DIRECTION) == BLOCK_DATA_DIR_FRONT || (block->data & BLOCK_DATA_DIRECTION) == BLOCK_DATA_DIR_BACK)
    {
        calcBlockVertices(v, x + BLOCK_PIXEL(1), y, z + BLOCK_PIXEL(2), BLOCK_PIXEL(6), BLOCK_PIXEL(2), BLOCK_PIXEL(4));
    }
    else
    {
        calcBlockVertices(v, x + BLOCK_PIXEL(2), y, z + BLOCK_PIXEL(1), BLOCK_PIXEL(4), BLOCK_PIXEL(2), BLOCK_PIXEL(6));
    }

    vec2 tex = getBlockTexture(BLOCK_COBBLESTONE, 0);
    float texX1 = PTCL(tex.x);
    float texY1 = PTCL(tex.y);
    vec2 faceTextures[5] = {
        {PTCH(tex.x + 6), PTCH(tex.y + 2)},
        {PTCH(tex.x + 6), PTCH(tex.y + 2)},
        {PTCH(tex.x + 6), PTCH(tex.y + 2)},
        {PTCH(tex.x + 6), PTCH(tex.y + 2)},
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
    calcBlockVertices(v, -BLOCK_PIXEL(1), -BLOCK_PIXEL(2.5f), -BLOCK_PIXEL(1), BLOCK_PIXEL(2), BLOCK_PIXEL(5), BLOCK_PIXEL(2));

    //Rotate lever
    float angle = (block->data & BLOCK_DATA_POWER) ? DEG_TO_RAD(45) : DEG_TO_RAD(-45);
    float xPos = x + BLOCK_PIXEL(4) + ((block->data & BLOCK_DATA_POWER) ? -BLOCK_PIXEL(2) : BLOCK_PIXEL(2));
    float zPos = z + BLOCK_PIXEL(4) + ((block->data & BLOCK_DATA_POWER) ? BLOCK_PIXEL(2) : -BLOCK_PIXEL(2));
    for(uint8_t i = 0; i < 8; i++)
    {
        if((block->data & BLOCK_DATA_DIRECTION) == BLOCK_DATA_DIR_FRONT || (block->data & BLOCK_DATA_DIRECTION) == BLOCK_DATA_DIR_BACK)
        {
            v[i] = rotatev3(v[i], (vec3) {0, 0, 1}, angle);

            v[i].x += xPos;
            v[i].y += y + BLOCK_PIXEL(3.1f);
            v[i].z += z + BLOCK_PIXEL(4);
        }
        else
        {
            v[i] = rotatev3(v[i], (vec3) {1, 0, 0}, angle);

            v[i].x += x + BLOCK_PIXEL(4);
            v[i].y += y + BLOCK_PIXEL(3.1f);
            v[i].z += zPos;
        }
    }

    tex = getBlockTexture(BLOCK_PLANKS, 0);
    texX1 = PTCL(tex.x);
    texY1 = PTCL(tex.y);
    vec2 faceTextures2[5] = {
        {PTCH(tex.x + 2), PTCH(tex.y + 5)},
        {PTCH(tex.x + 2), PTCH(tex.y + 5)},
        {PTCH(tex.x + 2), PTCH(tex.y + 5)},
        {PTCH(tex.x + 2), PTCH(tex.y + 5)},
        {PTCH(tex.x + 2), PTCH(tex.y + 2)},
    };
    
    for(uint8_t i = 0; i < 5; i++)
    {
        glTexCoord2f(faceTextures2[i].x, faceTextures2[i].y);
        glVectorV3(*(faces[i][0]));
        glTexCoord2f(texX1, faceTextures2[i].y);
        glVectorV3(*(faces[i][1]));
        glTexCoord2f(texX1, texY1);
        glVectorV3(*(faces[i][2]));
        glTexCoord2f(faceTextures2[i].x, texY1);
        glVectorV3(*(faces[i][3]));
    }
}

void drawPistonBase(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    vec2 texF0 = getBlockTexture(block->type, 0);
    vec2 texF1 = {texF0.x + 8, texF0.y + 8};
    vec2 texB0 = getBlockTexture(block->type, 2);
    vec2 texB1 = {texB0.x + 8, texB0.y + 8};
    vec2 texS0 = getBlockTexture(block->type, 1);
    vec2 texS1 = {texS0.x + 6, texS0.y + 8};

    vec2 textures[6][2] = {
        texS0, texS1,
        texS0, texS1,
        texF0, texF1,
        texB0, texB1,
        texS0, texS1,
        texS0, texS1
    };

    drawPartBlock(block->data & BLOCK_DATA_DIRECTION, x, y, z, BLOCK_PIXEL(6), false, textures);

    //We don't draw a rod part here, since the piston head takes care of it for us
    //This is okay since the base and head *always* appear together
}

void drawPistonHead(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    vec2 texF0 = getBlockTexture(block->type, 0);
    vec2 texF1 = {texF0.x + 8, texF0.y + 8};
    vec2 texS0 = getBlockTexture(block->type, 1);
    vec2 texS1 = {texS0.x + 8, texS0.y + 8};
    vec2 texS2 = {texS0.x + 2, texS0.y + 8};

    vec2 textures[6][2] = {
        texS0, texS2,
        texS0, texS2,
        texF0, texF1,
        texS0, texS1,
        texS0, texS2,
        texS0, texS2
    };

    drawPartBlock(block->data & BLOCK_DATA_DIRECTION, x, y, z, BLOCK_PIXEL(2), true, textures);

    //Draw piston rod (one block length to connect to the base)
    vec2 texR0 = texS0;
    vec2 texR1 = {texS0.x + 8, texS0.y + 2};

    //Calculate vertices
    uint8_t dir = block->data & BLOCK_DATA_DIRECTION;
    uint8_t front = dir == BLOCK_DATA_DIR_FRONT;
    uint8_t back = dir == BLOCK_DATA_DIR_BACK;
    uint8_t left = dir == BLOCK_DATA_DIR_LEFT;
    uint8_t right = dir == BLOCK_DATA_DIR_RIGHT;

    float x1 = x - BLOCK_PIXEL(2);
    float xS = BLOCK_SIZE;
    float z1 = z + BLOCK_PIXEL(3);
    float zS = BLOCK_PIXEL(2);
    //Back is covered by the default case
    if(front)
    {
        x1 = x + BLOCK_PIXEL(2);
    }
    else if(right)
    {
        x1 = x + BLOCK_PIXEL(3);
        xS = BLOCK_PIXEL(2);
        z1 = z - BLOCK_PIXEL(2);
        zS = BLOCK_SIZE;
    }
    else if(left)
    {
        x1 = x + BLOCK_PIXEL(3);
        xS = BLOCK_PIXEL(2);
        z1 = z + BLOCK_PIXEL(2);
        zS = BLOCK_SIZE;
    }

    vec3 v[8];
    calcBlockVertices(v, x1, y + BLOCK_PIXEL(3), z1, xS, BLOCK_PIXEL(2), zS);
    vec3* faces[6][4] = BLOCK_ALL_FACES(v);

    for(uint8_t i = 0; i < 6; i++)
    {
        vec2 tex1 = texR1;

        glTexCoord2f(PTCH(tex1.x), PTCH(tex1.y));
        glVectorV3(*(faces[i][0]));
        glTexCoord2f(PTCL(texR0.x), PTCH(tex1.y));
        glVectorV3(*(faces[i][1]));
        glTexCoord2f(PTCL(texR0.x), PTCL(texR0.y));
        glVectorV3(*(faces[i][2]));
        glTexCoord2f(PTCH(tex1.x), PTCL(texR0.y));
        glVectorV3(*(faces[i][3]));
    }
}