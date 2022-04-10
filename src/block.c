#include "block.h"

#include <GL/gl.h>

#include "engine/includes/3dMath.h"
#include "engine/util.h"

#define BLOCK_SIZE 1.0f
//Add 0.5 because OpenGL samples textures at the texel center, but we want the top-left corner (to get the entire pixel)
#define PTC(X) (((X) - 0.5f) / 256.0f)

#define glVectorV3(vec) glVertex3f((vec).x, (vec).y, (vec).z)

vec2 textures[16 * 16] = {
    {8, 0}, //Stone
    {16, 0}, //Dirt
    {24, 0}, //Grass Side
    {0, 8}, //Bedrock
    {64, 16} //Grass top
};

uint8_t normalBlockTextures[] = {
    [BLOCK_STONE] = 0,
    [BLOCK_DIRT] = 1,
    [BLOCK_BEDROCK] = 3
};

uint8_t orientedBlockTextures[][6] = {
    [BLOCK_GRASS] = {2, 2, 2, 2, 4, 1}
};

void drawNormalBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    //Precalculate all 8 vertices
    vec3 v1 = {x, y, z + BLOCK_SIZE};
    vec3 v2 = {x + BLOCK_SIZE, y, z + BLOCK_SIZE};
    vec3 v3 = {x + BLOCK_SIZE, y + BLOCK_SIZE, z + BLOCK_SIZE};
    vec3 v4 = {x, y + BLOCK_SIZE, z + BLOCK_SIZE};

    vec3 v5 = {x + BLOCK_SIZE, y, z};
    vec3 v6 = {x, y, z};
    vec3 v7 = {x, y + BLOCK_SIZE, z};
    vec3 v8 = {x + BLOCK_SIZE, y + BLOCK_SIZE, z};

    vec3* faces[6][4] = {
        {&v1, &v2, &v3, &v4},
        {&v5, &v6, &v7, &v8},
        {&v6, &v1, &v4, &v7},
        {&v2, &v5, &v8, &v3},
        {&v4, &v3, &v8, &v7},
        {&v6, &v5, &v2, &v1}
    };

    //Get texture position
    vec2 tex = textures[normalBlockTextures[block->type]];
    float texX1 = PTC(tex.x);
    float texX2 = PTC(tex.x + 8);
    float texY1 = PTC(tex.y);
    float texY2 = PTC(tex.y + 8);

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

void drawOrientedBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    //Precalculate all 8 vertices
    vec3 v1 = {x, y, z + BLOCK_SIZE};
    vec3 v2 = {x + BLOCK_SIZE, y, z + BLOCK_SIZE};
    vec3 v3 = {x + BLOCK_SIZE, y + BLOCK_SIZE, z + BLOCK_SIZE};
    vec3 v4 = {x, y + BLOCK_SIZE, z + BLOCK_SIZE};

    vec3 v5 = {x + BLOCK_SIZE, y, z};
    vec3 v6 = {x, y, z};
    vec3 v7 = {x, y + BLOCK_SIZE, z};
    vec3 v8 = {x + BLOCK_SIZE, y + BLOCK_SIZE, z};

    vec3* faces[6][4] = {
        {&v1, &v2, &v3, &v4},
        {&v5, &v6, &v7, &v8},
        {&v6, &v1, &v4, &v7},
        {&v2, &v5, &v8, &v3},
        {&v4, &v3, &v8, &v7},
        {&v6, &v5, &v2, &v1}
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

        glTexCoord2f(PTC(tex[i].x + 8), PTC(tex[i].y + 8));
        glVectorV3(*(faces[i][0]));
        glTexCoord2f(PTC(tex[i].x), PTC(tex[i].y + 8));
        glVectorV3(*(faces[i][1]));
        glTexCoord2f(PTC(tex[i].x), PTC(tex[i].y));
        glVectorV3(*(faces[i][2]));
        glTexCoord2f(PTC(tex[i].x + 8), PTC(tex[i].y));
        glVectorV3(*(faces[i][3]));

        occlusionCheck <<= 1;
    }
}

void drawBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    switch(block->type)
    {
        case BLOCK_GRASS:
        {
            drawOrientedBlock(block, x, y, z, occlusion);
            break;
        }
        default:
        {
            drawNormalBlock(block, x, y, z, occlusion);
            break;
        }
        //TODO: Special types
    }
}