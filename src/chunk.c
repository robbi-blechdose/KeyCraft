#include "chunk.h"

void generateDrawData(Chunk* chunk)
{
    chunk->drawList = glGenLists(1);
	glNewList(chunk->drawList, GL_COMPILE);
	glBegin(GL_QUADS);

    for(uint8_t i = 0; i < CHUNK_SIZE; i++)
    {
        for(uint8_t j = 0; j < CHUNK_SIZE; j++)
        {
            for(uint8_t k = 0; k < CHUNK_SIZE; k++)
            {
                if(CHUNK_BLOCK(chunk, i, j, k).type != BLOCK_AIR)
                {
                    //Simple block occlusion check
                    uint8_t occlusion = 0;
                    if((k + 1 < CHUNK_SIZE) && CHUNK_BLOCK(chunk, i, j, k + 1).type)
                    {
                        occlusion |= BS_FRONT;
                    }
                    if((k > 0) && CHUNK_BLOCK(chunk, i, j, k - 1).type)
                    {
                        occlusion |= BS_BACK;
                    }
                    if((i + 1 < CHUNK_SIZE) && CHUNK_BLOCK(chunk, i + 1, j, k).type)
                    {
                        occlusion |= BS_RIGHT;
                    }
                    if((i > 0) && CHUNK_BLOCK(chunk, i - 1, j, k).type)
                    {
                        occlusion |= BS_LEFT;
                    }
                    if((j + 1 < CHUNK_SIZE) && CHUNK_BLOCK(chunk, i, j + 1, k).type)
                    {
                        occlusion |= BS_TOP;
                    }
                    if((j > 0) && CHUNK_BLOCK(chunk, i, j - 1, k).type)
                    {
                        occlusion |= BS_BOTTOM;
                    }
                    drawBlock(&CHUNK_BLOCK(chunk, i, j, k), i, j, k, occlusion);
                }
            }
        }
    }

	glEnd();
	glEndList();
}

void calcChunk(Chunk* chunk, uint32_t ticks)
{
    if(chunk->modified)
    {
        chunk->modified = 0;
        //glDeleteList(chunk->drawList);
        generateDrawData(chunk);
    }
}

void drawChunk(Chunk* chunk)
{
    //TODO: Translate in world space?
    glCallList(chunk->drawList);
}

void destroyChunk(Chunk* chunk)
{
    glDeleteList(chunk->drawList);
}