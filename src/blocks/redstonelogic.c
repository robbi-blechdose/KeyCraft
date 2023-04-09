#include "redstonelogic.h"

#include "../world.h"
#include "blockutils.h"

bool hasAdjacentPower(ChunkPos chunk, uint8_t x, uint8_t y, uint8_t z, bool onlySource)
{
    BlockPos blockPos = {chunk, x, y, z};

    for(uint8_t i = 0; i < 4; i++)
    {
        blockPos.x += adjacentDiffs[i][0];
        blockPos.z += adjacentDiffs[i][1];
        Block* block = getWorldBlock(&blockPos);
        if(block != NULL)
        {
            if(block->type == BLOCK_REDSTONE_REPEATER && (block->data & BLOCK_DATA_POWER))
            {
                BlockPos pos = blockPos;
                getBlockPosByInverseDirection(block->data & BLOCK_DATA_DIRECTION, &pos);
                normalizeBlockPos(&pos);
                if(pos.chunk.x == chunk.x && pos.chunk.z == chunk.z && pos.x == x && pos.z == z)
                {
                    return true;
                }
            }
            else if(block->data & BLOCK_DATA_POWER)
            {
                if(!onlySource || (onlySource && block->type != BLOCK_REDSTONE_WIRE))
                {
                    return true;
                }
            }
            else if(block->type == BLOCK_COMPUTER)
            {
                //Hack to allow computers to output directional signals
                ComputerData* computer = getWorldChunk(&blockPos)->computers[GET_COMPUTER_INDEX(block->data)];

                if(computer != NULL)
                {
                    uint8_t out = LOW_NIBBLE(computer->io);

                    //Maps block rotation to input bit indices
                    uint8_t indexMapping[4] = {2, 0, 3, 1};
                    uint8_t index = (i + indexMapping[(block->data & BLOCK_DATA_DIRECTION) >> 6]) % 4;

                    if(out & (1 << index))
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool hasCircuitPowerSource(ChunkPos chunk, uint8_t x, uint8_t y, uint8_t z)
{
    if(hasAdjacentPower(chunk, x, y, z, true))
    {
        return true;
    }

    BlockPos blockPos = {chunk, x, y, z};
    Block* bp = getWorldBlock(&blockPos);
    if(bp->data & BLOCK_DATA_VISITED)
    {
        return false;
    }

    bp->data |= BLOCK_DATA_VISITED;

    for(int8_t i = -1; i <= 1; i++)
    {
        blockPos.y = ((int8_t) y) + i;
        blockPos.x = x;
        blockPos.z = z;
        blockPos.chunk = chunk;

        for(uint8_t j = 0; j < 4; j++)
        {
            blockPos.x += adjacentDiffs[j][0];
            blockPos.z += adjacentDiffs[j][1];
            Block* block = getWorldBlock(&blockPos);
            if(block != NULL && block->type == BLOCK_REDSTONE_WIRE)
            {
                if(hasCircuitPowerSource(blockPos.chunk, blockPos.x, blockPos.y, blockPos.z))
                {
                    bp->data &= ~BLOCK_DATA_VISITED;
                    return true;
                }
            }
        }
    }

    bp->data &= ~BLOCK_DATA_VISITED;
    return false;
}

void updateCircuit(ChunkPos chunk, uint8_t x, uint8_t y, uint8_t z, bool powered)
{
    BlockPos blockPos = {chunk, x, y, z};

    //Set power state
    Block block = *getWorldBlock(&blockPos);
    if(powered)
    {
        block.data |= (BLOCK_DATA_TEXTURE1 | BLOCK_DATA_POWER);
    }
    else
    {
        block.data &= ~(BLOCK_DATA_TEXTURE | BLOCK_DATA_POWER);
    }
    setWorldBlock(&blockPos, block);

    //Update rest of the circuit
    for(int8_t i = -1; i <= 1; i++)
    {
        blockPos.y = ((int8_t) y) + i;
        blockPos.x = x;
        blockPos.z = z;
        blockPos.chunk = chunk;

        for(uint8_t j = 0; j < 4; j++)
        {
            blockPos.x += adjacentDiffs[j][0];
            blockPos.z += adjacentDiffs[j][1];
            Block* block = getWorldBlock(&blockPos);
            if(block != NULL && block->type == BLOCK_REDSTONE_WIRE)
            {
                if(powered && !(block->data & BLOCK_DATA_POWER) || !powered && (block->data & BLOCK_DATA_POWER))
                {
                    updateCircuit(blockPos.chunk, blockPos.x, blockPos.y, blockPos.z, powered);
                }
            }
        }
    }
}

void tickRedstoneWire(Chunk* chunk, Block* block, uint8_t x, uint8_t y, uint8_t z)
{
    if(block->data & BLOCK_DATA_POWER)
    {
        if(!hasCircuitPowerSource(chunk->position, x, y, z))
        {
            //We used to have power but don't now
            updateCircuit(chunk->position, x, y, z, false);
        }
    }
    else
    {
        if(hasCircuitPowerSource(chunk->position, x, y, z))
        {
            //We used to not have power but do now
            updateCircuit(chunk->position, x, y, z, true);
        }
    }
}

void tickRedstoneTorch(Chunk* chunk, Block* block, uint8_t x, uint8_t y, uint8_t z)
{
    //Check if the torch is powered from below - if so, turn it off
    bool powerBelow = hasAdjacentPower(chunk->position, x, y - 1, z, false);
    //Check if something changed
    if((block->data & BLOCK_DATA_POWER) && powerBelow)
    {
        block->data &= ~BLOCK_DATA_POWER;
        block->data &= ~BLOCK_DATA_TEXTURE;
        CHUNK_SET_FLAG(chunk, CHUNK_MODIFIED);
    }
    else if((block->data & BLOCK_DATA_POWER) == 0 && !powerBelow)
    {
        block->data |= BLOCK_DATA_POWER;
        block->data |= BLOCK_DATA_TEXTURE1;
        CHUNK_SET_FLAG(chunk, CHUNK_MODIFIED);
    }
}

void tickRedstoneRepeater(Chunk* chunk, Block* block, uint8_t x, uint8_t y, uint8_t z)
{
    bool power = false;

    BlockPos blockPos = {chunk->position, x, y, z};
    getBlockPosByDirection(block->data & BLOCK_DATA_DIRECTION, &blockPos);

    Block* wBlock = getWorldBlock(&blockPos);
    if(wBlock != NULL)
    {
        if(wBlock->data & BLOCK_DATA_POWER)
        {
            power = true;
        }
    }

    if((block->data & BLOCK_DATA_POWER) == 0 && power)
    {
        block->data |= BLOCK_DATA_POWER;
        block->data |= BLOCK_DATA_TEXTURE1;
        CHUNK_SET_FLAG(chunk, CHUNK_MODIFIED);
    }
    else if((block->data & BLOCK_DATA_POWER) && !power)
    {
        block->data &= ~BLOCK_DATA_POWER;
        block->data &= ~BLOCK_DATA_TEXTURE;
        CHUNK_SET_FLAG(chunk, CHUNK_MODIFIED);
    }
}