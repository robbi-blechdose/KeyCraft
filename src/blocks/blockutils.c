#include "blockutils.h"

#include "block.h"

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