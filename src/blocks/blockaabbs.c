#include "blockaabbs.h"

AABB getAABBForBlock(Block* block, vec3 position)
{
    switch(block->type)
    {
        case BLOCK_DOOR:
        {
            uint8_t front = (block->data & BLOCK_DATA_DIRECTION) == BLOCK_DATA_DIR_FRONT;
            uint8_t back = (block->data & BLOCK_DATA_DIRECTION) == BLOCK_DATA_DIR_BACK;
            uint8_t left = (block->data & BLOCK_DATA_DIRECTION) == BLOCK_DATA_DIR_LEFT;
            uint8_t right = (block->data & BLOCK_DATA_DIRECTION) == BLOCK_DATA_DIR_RIGHT;
            float xMin = (front || left || right) ? 0 : 1 - DOOR_WIDTH;
            float xMax = front ? DOOR_WIDTH : BLOCK_SIZE;
            float zMin = (left || front || back) ? 0 : 1 - DOOR_WIDTH;
            float zMax = left ? DOOR_WIDTH : BLOCK_SIZE;

            return (AABB) {.min = (vec3) {position.x + xMin, position.y, position.z + zMin},
                           .max = (vec3) {position.x + xMax, position.y + BLOCK_SIZE, position.z + zMax}};
        }
        default:
        {
            return (AABB) {.min = position,
                           .max = (vec3) {position.x + BLOCK_SIZE, position.y + BLOCK_SIZE, position.z + BLOCK_SIZE}};
        }
    }
}