#include "block.h"

#include <GL/gl.h>

#include "engine/includes/3dMath.h"
#include "engine/util.h"

//Add 0.5 because OpenGL samples textures at the texel center, but we want the top-left corner (to get the entire pixel)
#define PTC(X) (((X) - 0.5f) / 255.0f)

#define glVectorV3(vec) glVertex3f((vec).x, (vec).y, (vec).z)

const vec2 textures[16 * 16] = {
    {8, 0}, //Stone
    {16, 0}, //Dirt
    {24, 0}, //Grass Side
    {32, 0}, //Planks
    {96, 0}, //Red flowers
    {0, 8}, //Bedrock
    {64, 16} //Grass top
};

const uint8_t normalBlockTextures[] = {
    [BLOCK_STONE] = 0,
    [BLOCK_DIRT] = 1,
    [BLOCK_PLANKS] = 3,
    [BLOCK_BEDROCK] = 5
};

const uint8_t orientedBlockTextures[][6] = {
    [BLOCK_GRASS] = {2, 2, 2, 2, 6, 1}
};

const uint8_t xBlockTextures[] = {
    [BLOCK_FLOWER] = 4
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

//TODO: Rename
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

void drawXBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    if(occlusion & BS_ALL)
    {
        return;
    }

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
        {&v1, &v5, &v8, &v4},
        {&v6, &v2, &v3, &v7},
    };

    //Get texture position
    vec2 tex = textures[xBlockTextures[block->type]];
    float texX1 = PTC(tex.x);
    float texX2 = PTC(tex.x + 8);
    float texY1 = PTC(tex.y);
    float texY2 = PTC(tex.y + 8);

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

void drawBlock(Block* block, uint8_t x, uint8_t y, uint8_t z, uint8_t occlusion)
{
    //Precalculate corner vertices (all 8 corners of a cube)
    vec3 cornerVerts[8] = {
        {x, y, z + BLOCK_SIZE},
        {x + BLOCK_SIZE, y, z + BLOCK_SIZE},
        {x + BLOCK_SIZE, y + BLOCK_SIZE, z + BLOCK_SIZE},
        {x, y + BLOCK_SIZE, z + BLOCK_SIZE},

        {x + BLOCK_SIZE, y, z},
        {x, y, z},
        {x, y + BLOCK_SIZE, z},
        {x + BLOCK_SIZE, y + BLOCK_SIZE, z}
    };

    switch(block->type)
    {
        case BLOCK_GRASS:
        {
            drawOrientedBlock(block, x, y, z, occlusion);
            break;
        }
        case BLOCK_FLOWER:
        {
            drawXBlock(block, x, y, z, occlusion);
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

uint8_t isFullBlock(BlockType type)
{
    switch(type)
    {
        case BLOCK_AIR:
        case BLOCK_FLOWER:
        {
            return 0;
        }
        default:
        {
            return 1;
        }
    }
}