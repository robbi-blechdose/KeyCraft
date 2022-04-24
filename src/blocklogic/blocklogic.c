#include "blocklogic.h"

void tickBlock(Chunk* chunk, Block* block)
{
    switch(block->type)
    {
        case BLOCK_WHEAT:
        {
            chunk->modified = 1;
            
            if((block->data & BLOCK_DATA_TEXTURE) != BLOCK_DATA_TEXTURE3)
            {
                if((block->data & BLOCK_DATA_COUNTER) != BLOCK_DATA_COUNTER)
                {
                    block->data += BLOCK_DATA_COUNTER1;
                }
                else
                {
                    (block->data)++;
                    block->data &= ~BLOCK_DATA_COUNTER; //Clear counter
                }
            }
            break;
        }
    }
}

void tickChunk(Chunk* chunk)
{
    //Don't tick chunk if we're also recalculating the geometry
    if(!chunk->modified)
    {
        for(uint8_t i = 0; i < CHUNK_SIZE; i++)
        {
            for(uint8_t j = 0; j < CHUNK_SIZE; j++)
            {
                for(uint8_t k = 0; k < CHUNK_SIZE; k++)
                {
                    tickBlock(chunk, &CHUNK_BLOCK(chunk, i, j, k));
                }
            }
        }
    }
}