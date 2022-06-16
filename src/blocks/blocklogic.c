#include "blocklogic.h"

#include "../world.h"

#include "../engine/util.h"

uint8_t hasAdjacentWater(Chunk* chunk, uint8_t x, uint8_t y, uint8_t z)
{
    BlockPos blockPos = {.chunk = chunk->position, .x = x - 1, .y = y, .z = z};
    uint8_t result = getWorldBlock(&blockPos)->type == BLOCK_WATER; //x-1,z
    blockPos.x += 2;
    result += getWorldBlock(&blockPos)->type == BLOCK_WATER; //x+1,z
    blockPos.x--;
    blockPos.z++;
    result += getWorldBlock(&blockPos)->type == BLOCK_WATER; //x,z+1
    blockPos.z -= 2;
    result += getWorldBlock(&blockPos)->type == BLOCK_WATER; //x,z-1

    return result;
}

uint8_t getAdjacentPower(Chunk* chunk, uint8_t x, uint8_t y, uint8_t z)
{
    uint8_t maxPower;
    uint8_t tempPower;

    BlockPos blockPos = {.chunk = chunk->position, .x = x - 1, .y = y, .z = z};
    maxPower = getWorldBlock(&blockPos)->data & BLOCK_DATA_POWER; //x-1,z
    blockPos.x += 2;
    tempPower = getWorldBlock(&blockPos)->data & BLOCK_DATA_POWER; //x+1,z
    maxPower = tempPower > maxPower ? tempPower : maxPower;
    blockPos.x--;
    blockPos.z++;
    tempPower = getWorldBlock(&blockPos)->data & BLOCK_DATA_POWER; //x,z+1
    maxPower = tempPower > maxPower ? tempPower : maxPower;
    blockPos.z -= 2;
    tempPower = getWorldBlock(&blockPos)->data & BLOCK_DATA_POWER; //x,z-1
    maxPower = tempPower > maxPower ? tempPower : maxPower;

    return maxPower;
}

void tickBlock(Chunk* chunk, Block* block, uint8_t x, uint8_t y, uint8_t z)
{
    switch(block->type)
    {
        case BLOCK_WHEAT:
        {
            if((block->data & BLOCK_DATA_TEXTURE) != BLOCK_DATA_TEXTURE3)
            {
                //Only grow wheat if there's water around the block below it (and a random fires to add variety)
                if(hasAdjacentWater(chunk, x, y - 1, z) && randr(100) < 20)
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
            }
            break;
        }
        case BLOCK_REDSTONE_LAMP:
        {
            uint8_t adjacentPower = getAdjacentPower(chunk, x, y, z);
            //Check if something changed
            if((block->data & BLOCK_DATA_TEXTURE) == 0 && adjacentPower)
            {
                block->data |= BLOCK_DATA_TEXTURE1;
                chunk->modified = 1;
            }
            else if((block->data & BLOCK_DATA_TEXTURE) == BLOCK_DATA_TEXTURE1 && adjacentPower == 0)
            {
                block->data &= ~BLOCK_DATA_TEXTURE;
                chunk->modified = 1;
            }
            //else no change
            break;
        }
        case BLOCK_REDSTONE_WIRE:
        {
            uint8_t adjacentPower = getAdjacentPower(chunk, x, y, z);
            uint8_t blockPower = (block->data & BLOCK_DATA_POWER);
            //Check if something changed
            if(adjacentPower != blockPower + BLOCK_DATA_POWER1 && !(adjacentPower == 0 && blockPower == 0))
            {
                if(blockPower == 0)
                {
                    //Used to be off, turn on
                    block->data |= BLOCK_DATA_TEXTURE1;
                    chunk->modified = 1;
                }

                block->data &= ~BLOCK_DATA_POWER;
                if(adjacentPower > 0)
                {
                    block->data |= adjacentPower - BLOCK_DATA_POWER1;
                }

                if((block->data & BLOCK_DATA_POWER) == 0)
                {
                    //Used to be on, turn off
                    block->data &= ~BLOCK_DATA_TEXTURE;
                    chunk->modified = 1;
                }
            }
            break;
        }
        case BLOCK_REDSTONE_TORCH:
        {
            uint8_t oldState = block->data & BLOCK_DATA_POWER;
            //Check if the torch is powered from below - if so, turn it off
            uint8_t powerBelow = 0; //TODO
            //Check if something changed
            if(powerBelow == 0 && (block->data & BLOCK_DATA_POWER) > 0)
            {
                block->data |= BLOCK_DATA_POWER;
                block->data |= BLOCK_DATA_TEXTURE1;
                chunk->modified = 1;
            }
            else if(powerBelow > 0 && (block->data & BLOCK_DATA_POWER) == 0)
            {
                block->data &= ~BLOCK_DATA_POWER;
                block->data &= ~BLOCK_DATA_TEXTURE;
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