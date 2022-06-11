#include "blocklogic.h"

#include "../engine/util.h"

uint8_t getAdjacentPower(Chunk* chunk, uint8_t x, uint8_t y, uint8_t z)
{
    //TODO: Also check blocks from adjacent chunks if necessary
    return ((CHUNK_BLOCK(chunk, x + 1, y, z).data & BLOCK_DATA_POWER) ||
            (CHUNK_BLOCK(chunk, x - 1, y, z).data & BLOCK_DATA_POWER) ||
            (CHUNK_BLOCK(chunk, x, y, z + 1).data & BLOCK_DATA_POWER) ||
            (CHUNK_BLOCK(chunk, x, y, z - 1).data & BLOCK_DATA_POWER)) ? BLOCK_DATA_POWERP : 0;
}

void tickBlock(Chunk* chunk, Block* block, uint8_t x, uint8_t y, uint8_t z)
{
    switch(block->type)
    {
        case BLOCK_WHEAT:
        {
            if((block->data & BLOCK_DATA_TEXTURE) != BLOCK_DATA_TEXTURE3 && randr(100) < 20)
            {
                if((block->data & BLOCK_DATA_COUNTER) != BLOCK_DATA_COUNTER)
                {
                    block->data += BLOCK_DATA_COUNTER1;
                }
                else
                {
                    (block->data)++;
                    block->data &= ~BLOCK_DATA_COUNTER; //Clear counter
                    chunk->modified = 1;
                }
            }
            break;
        }
        case BLOCK_REDSTONE_LAMP:
        {
            uint8_t adjacentPower = getAdjacentPower(chunk, x, y, z);
            //Check if something changed
            if((block->data & BLOCK_DATA_POWER) != adjacentPower)
            {
                if(adjacentPower)
                {
                    block->data |= BLOCK_DATA_POWERP;
                    block->data |= BLOCK_DATA_TEXTURE1;
                }
                else
                {
                    block->data &= ~BLOCK_DATA_POWER;
                    block->data &= ~BLOCK_DATA_TEXTURE;
                }
                chunk->modified = 1;
            }
            break;
        }
        case BLOCK_REDSTONE_WIRE:
        {
            //TODO
            break;
        }
        case BLOCK_REDSTONE_TORCH:
        {
            uint8_t oldState = block->data & BLOCK_DATA_POWER;
            //Check if the torch is powered from below - if so, turn it off
            uint8_t powerBelow = 0; //TODO
            //Check if something changed
            //TOOD
            if(1) //!((!powerBelow && !oldState) || (powerBelow && oldState)))
            {
                if(!powerBelow)
                {
                    block->data |= BLOCK_DATA_POWERA;
                    block->data |= BLOCK_DATA_TEXTURE1;
                }
                else
                {
                    block->data &= ~BLOCK_DATA_POWER;
                    block->data &= ~BLOCK_DATA_TEXTURE;
                }
                chunk->modified = 1;
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
                    tickBlock(chunk, &CHUNK_BLOCK(chunk, i, j, k), i, j, k);
                }
            }
        }
    }
}