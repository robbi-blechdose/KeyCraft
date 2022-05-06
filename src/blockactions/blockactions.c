#include "blockactions.h"

void blockAction(Chunk* chunk, Block* block)
{
    if(block->type == BLOCK_DOOR)
    {
        //TODO: rotate block
        
        chunk->modified = 1;
    }
}