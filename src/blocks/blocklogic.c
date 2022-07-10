#include "blocklogic.h"

#include "../engine/util.h"
#include "../engine/audio.h"

#include "../world.h"
#include "blockutils.h"
#include "../sfx.h"

/**
 * Differences from the center position to the adjacent blocks:
 *  3
 * 0X1
 *  2
 */
int8_t adjacentDiffs[4][2] = {
    {-1, 0},
    { 2, 0},
    {-1, 1},
    {0, -2}
};

uint8_t hasAdjacentWater(Chunk* chunk, uint8_t x, uint8_t y, uint8_t z)
{
    BlockPos blockPos = {chunk->position, x, y, z};
    uint8_t result = 0;

    for(uint8_t i = 0; i < 4; i++)
    {
        blockPos.x += adjacentDiffs[i][0];
        blockPos.z += adjacentDiffs[i][1];
        Block* block = getWorldBlock(&blockPos);
        if(block != NULL && block->type == BLOCK_WATER)
        {
            result++;
        }
    }

    return result;
}

uint8_t getAdjacentPower(Chunk* chunk, uint8_t x, uint8_t y, uint8_t z, bool onlyWire)
{
    uint8_t maxPower = 0;
    
    BlockPos blockPos = {chunk->position, x, y, z};

    for(uint8_t i = 0; i < 4; i++)
    {
        blockPos.x += adjacentDiffs[i][0];
        blockPos.z += adjacentDiffs[i][1];
        Block* block = getWorldBlock(&blockPos);
        if(block != NULL)
        {
            if(!onlyWire || (onlyWire && block->type == BLOCK_REDSTONE_WIRE))
            {
                uint8_t tempPower = block->data & BLOCK_DATA_POWER;
                maxPower = tempPower > maxPower ? tempPower : maxPower;
            }
        }
    }

    return maxPower;
}

void explodeTNT(ChunkPos chunk, uint8_t x, uint8_t y, uint8_t z)
{
    BlockPos pos = {chunk, x, y, z};
                
    //Remove this TNT block
    setWorldBlock(&pos, (Block) {BLOCK_AIR, 0});

    //Remove blocks in explosion radius
    for(int8_t i = x - TNT_RADIUS; i <= x + TNT_RADIUS; i++)
    {
        for(int8_t j = y - TNT_RADIUS; j <= y + TNT_RADIUS; j++)
        {
            for(int8_t k = z - TNT_RADIUS; k <= z + TNT_RADIUS; k++)
            {
                if(sqrtf(powf(i - x, 2) + powf(j - y, 2) + powf(k - z, 2)) <= TNT_RADIUS)
                {
                    pos.chunk = chunk;
                    pos.x = i;
                    pos.y = j;
                    pos.z = k;
                    Block* block = getWorldBlock(&pos);
                    if(block != NULL)
                    {
                        if(block->type == BLOCK_TNT)
                        {
                            //Explode other TNT blocks as well (chain reaction)
                            explodeTNT(pos.chunk, pos.x, pos.y, pos.z);
                        }
                        else if(block->type != BLOCK_BEDROCK)
                        {
                            setWorldBlock(&pos, (Block) {BLOCK_AIR, 0});
                        }
                    }
                }
            }
        }
    }
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
            uint8_t adjacentPower = getAdjacentPower(chunk, x, y, z, false);
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
            //Check for power around block
            uint8_t adjacentPower = getAdjacentPower(chunk, x, y, z, false);
            uint8_t blockPower = (block->data & BLOCK_DATA_POWER);

            //Check specifically for redstone wire one block higher and lower
            uint8_t tempPower = getAdjacentPower(chunk, x, y - 1, z, true);
            adjacentPower = tempPower > adjacentPower ? tempPower : adjacentPower;
            tempPower = getAdjacentPower(chunk, x, y + 1, z, true);
            adjacentPower = tempPower > adjacentPower ? tempPower : adjacentPower;

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
            uint8_t powerBelow = getAdjacentPower(chunk, x, y - 1, z, false);
            //Check if something changed
            if((block->data & BLOCK_DATA_POWER) && powerBelow)
            {
                block->data &= ~BLOCK_DATA_POWER;
                block->data &= ~BLOCK_DATA_TEXTURE;
                chunk->modified = 1;
            }
            else if((block->data & BLOCK_DATA_POWER) == 0 && powerBelow == 0)
            {
                block->data |= BLOCK_DATA_POWER;
                block->data |= BLOCK_DATA_TEXTURE1;
                chunk->modified = 1;
            }
            break;
        }
        case BLOCK_TNT:
        {
            if(getAdjacentPower(chunk, x, y, z, false))
            {
                explodeTNT(chunk->position, x, y, z);
                playSample(SFX_TNT);
            }
            break;
        }
        case BLOCK_PISTON:
        {
            //Piston block can only extend
            if(getAdjacentPower(chunk, x, y, z, false))
            {
                uint8_t dir = block->data & BLOCK_DATA_DIRECTION;
                BlockPos pos = {chunk->position, x, y, z};
                BlockPos pos2;
                
                getBlockPosByDirection(dir, &pos);
                Block* front = getWorldBlock(&pos);
                pos2 = pos;
                getBlockPosByDirection(dir, &pos2);
                Block* front2 = getWorldBlock(&pos2);

                if(front != NULL)
                {
                    if(front->type == BLOCK_AIR)
                    {
                        //No block in front of piston, extend
                        block->type = BLOCK_PISTON_BASE;
                        setWorldBlock(&pos, (Block) {BLOCK_PISTON_HEAD, dir});
                        chunk->modified = 1;
                    }
                    else if(front2->type == BLOCK_AIR)
                    {
                        //One block in front of piston, extend and push
                        Block temp = *front;
                        block->type = BLOCK_PISTON_BASE;
                        setWorldBlock(&pos, (Block) {BLOCK_PISTON_HEAD, dir});
                        setWorldBlock(&pos2, temp);
                        chunk->modified = 1;
                    }
                }
            }
            break;
        }
        case BLOCK_PISTON_BASE:
        {
            //Piston base block can only retract
            if(!getAdjacentPower(chunk, x, y, z, false))
            {
                uint8_t dir = block->data & BLOCK_DATA_DIRECTION;
                BlockPos pos = {chunk->position, x, y, z};
                BlockPos pos2;
                
                getBlockPosByDirection(dir, &pos);
                Block* front = getWorldBlock(&pos);
                pos2 = pos;
                getBlockPosByDirection(dir, &pos2);
                Block* front2 = getWorldBlock(&pos2);
                
                if(front != NULL && front2 != NULL)
                {
                    if(front2->type == BLOCK_AIR)
                    {
                        //No block in front of piston, retract
                        block->type = BLOCK_PISTON;
                        setWorldBlock(&pos, (Block) {BLOCK_AIR, 0});
                        chunk->modified = 1;
                    }
                    else
                    {
                        //One block in front of piston, retract and pull
                        Block temp = *front2;
                        block->type = BLOCK_PISTON;
                        setWorldBlock(&pos, temp);
                        setWorldBlock(&pos2, (Block) {BLOCK_AIR, 0});
                        chunk->modified = 1;
                    }
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
                    tickBlock(chunk, &CHUNK_BLOCK(chunk, i, j, k), i, j, k);
                }
            }
        }
    }
}