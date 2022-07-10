#include "blockutils.h"

#include "block.h"

int8_t adjacentDiffs[4][2] = {
    {-1, 0},
    { 2, 0},
    {-1, 1},
    {0, -2}
};

void getBlockPosByDirection(uint8_t dir, BlockPos* pos)
{
    switch(dir)
    {
        case BLOCK_DATA_DIR_FRONT:
        {
            pos->x++;
            break;
        }
        case BLOCK_DATA_DIR_BACK:
        {
            pos->x--;
            break;
        }
        case BLOCK_DATA_DIR_LEFT:
        {
            pos->z++;
            break;
        }
        case BLOCK_DATA_DIR_RIGHT:
        {
            pos->z--;
            break;
        }
    }
}