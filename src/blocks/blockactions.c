#include "blockactions.h"

#include "../engine/audio.h"

#include "../sfx.h"

void actDoor(Chunk* chunk, Block* block)
{
    uint8_t rotation;

    if(block->data & BLOCK_DATA_STATE)
    {
        //Door is closed, open it
        switch(block->data & BLOCK_DATA_DIRECTION)
        {
            case BLOCK_DATA_DIR_FRONT:
            {
                rotation = BLOCK_DATA_DIR_RIGHT;
                break;
            }
            case BLOCK_DATA_DIR_RIGHT:
            {
                rotation = BLOCK_DATA_DIR_BACK;
                break;
            }
            case BLOCK_DATA_DIR_BACK:
            {
                rotation = BLOCK_DATA_DIR_LEFT;
                break;
            }
            case BLOCK_DATA_DIR_LEFT:
            {
                rotation = BLOCK_DATA_DIR_FRONT;
                break;
            }
        }
    }
    else
    {
        //Door is open, close it
        switch(block->data & BLOCK_DATA_DIRECTION)
        {
            case BLOCK_DATA_DIR_FRONT:
            {
                rotation = BLOCK_DATA_DIR_LEFT;
                break;
            }
            case BLOCK_DATA_DIR_LEFT:
            {
                rotation = BLOCK_DATA_DIR_BACK;
                break;
            }
            case BLOCK_DATA_DIR_BACK:
            {
                rotation = BLOCK_DATA_DIR_RIGHT;
                break;
            }
            case BLOCK_DATA_DIR_RIGHT:
            {
                rotation = BLOCK_DATA_DIR_FRONT;
                break;
            }
        }
    }

    //Set new direction
    block->data = (block->data & ~BLOCK_DATA_DIRECTION) + rotation;
    //Invert state
    block->data ^= BLOCK_DATA_STATE;
    
    CHUNK_SET_FLAG(chunk, CHUNK_MODIFIED);

    playSample(SFX_DOOR);
}

bool actBlock(Chunk* chunk, Block* block)
{
    switch(block->type)
    {
        case BLOCK_DOOR:
        {
            actDoor(chunk, block);
            return true;
        }
        case BLOCK_LEVER:
        {
            if(block->data & BLOCK_DATA_POWER)
            {
                block->data &= ~BLOCK_DATA_POWER;
            }
            else
            {
                block->data |= BLOCK_DATA_POWER;
            }
            CHUNK_SET_FLAG(chunk, CHUNK_MODIFIED);

            playSample(SFX_LEVER);

            return true;
        }
        case BLOCK_FURNACE:
        {
            //Turn furnace on (if off)
            if(!(block->data & BLOCK_DATA_TEXTURE))
            {
                block->data |= BLOCK_DATA_TEXTURE1;
                CHUNK_SET_FLAG(chunk, CHUNK_MODIFIED);
            }

            return true;
        }
        case BLOCK_NOTEBLOCK:
        {
            //Cycle through notes
            uint8_t temp = (block->data + BLOCK_DATA_NOTEBLOCK1) & BLOCK_DATA_NOTEBLOCK;
            block->data &= ~BLOCK_DATA_NOTEBLOCK;
            block->data |= temp;

            return true;
        }
        default:
        {
            return false;
        }
    }
}