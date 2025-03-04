#include "gameloop.h"

#include "fk-engine-core/input.h"

#include "world.h"
#include "blocks/blockutils.h"
#include "blocks/block.h"
#include "inventory.h"
#include "gui/programming.h"

ComputerData* programmingComputer;

inline void tryPlaceBlockOrInteract(Player* player, State* state, BlockPos* block, AABBSide result)
{
    if(actWorldBlock(block))
    {
        return;
    }
    
    if(getWorldBlock(block)->type == BLOCK_COMPUTER)
    {
        programmingComputer = getWorldChunk(block)->computers[GET_COMPUTER_INDEX(getWorldBlock(block)->data)];
        *state = STATE_PROGRAMMING;
        return;
    }

    //Calc position
    switch(result)
    {
        case AABB_FRONT:
        {
            block->z -= BLOCK_SIZE;
            break;
        }
        case AABB_BACK:
        {
            block->z += BLOCK_SIZE;
            break;
        }
        case AABB_LEFT:
        {
            block->x -= BLOCK_SIZE;
            break;
        }
        case AABB_RIGHT:
        {
            block->x += BLOCK_SIZE;
            break;
        }
        case AABB_BOTTOM:
        {
            block->y -= BLOCK_SIZE;
            break;
        }
        case AABB_TOP:
        {
            block->y += BLOCK_SIZE;
            break;
        }
        default:
        {
            break;
        }
    }

    if(getWorldBlock(block) == NULL || getWorldBlock(block)->type != BLOCK_AIR)
    {
        return;
    }

    //Place new block
    BlockPos below = *block;
    below.y -= BLOCK_SIZE;
    BlockPos above = *block;
    above.y += BLOCK_SIZE;
    Block toPlace = getHotbarSelection();

    if(!canPlaceBlock(toPlace.type, getWorldBlock(&below)->type) ||
        (toPlace.type == BLOCK_DOOR && (getWorldBlock(&above) == NULL || getWorldBlock(&above)->type != BLOCK_AIR))) //Check if we can place the door upper (yes, this is a special case)
    {
        return;
    }

    if(isBlockOriented(toPlace.type))
    {
        uint8_t orientation = BLOCK_DATA_DIR_RIGHT;
        float rotation = player->rotation.y - M_PI_4;
        clampAngle(&rotation);

        if(rotation < M_PI_2)
        {
            orientation = BLOCK_DATA_DIR_FRONT;
        }
        else if(rotation < M_PI)
        {
            orientation = BLOCK_DATA_DIR_LEFT;
        }
        else if(rotation < M_PI + M_PI_2)
        {
            orientation = BLOCK_DATA_DIR_BACK;
        }
        //4th case is covered by the original assignment
        toPlace.data |= orientation;
    }

    setWorldBlock(block, toPlace);
    //Check if the block intersects with the player. If so, don't place it
    if(playerIntersectsWorld(player))
    {
        //Remove door upper as well (yes, this is a special case)
        if(toPlace.type == BLOCK_DOOR)
        {
            setWorldBlock(&above, (Block) {BLOCK_AIR, 0});
        }
        else if(toPlace.type == BLOCK_COMPUTER)
        {
            //Remove computer data
            free(getWorldChunk(block)->computers[GET_COMPUTER_INDEX(getWorldBlock(block)->data)]);
            getWorldChunk(block)->computers[GET_COMPUTER_INDEX(getWorldBlock(block)->data)] = NULL;
        }
        setWorldBlock(block, (Block) {BLOCK_AIR, 0});
    }
}

inline void tryRemoveBlock(BlockPos* block)
{
    Block* toRemove = getWorldBlock(block);
    
    if(toRemove->type == BLOCK_BEDROCK)
    {
        return;
    }
    
    //Remove other door half (yes, this is a special case)
    if(toRemove->type == BLOCK_DOOR)
    {
        if(toRemove->data & BLOCK_DATA_PART)
        {
            //This is the upper part, remove lower
            block->y--;
            setWorldBlock(block, (Block) {BLOCK_AIR, 0});
            block->y++;
        }
        else
        {
            //This is the lower part, remove upper
            block->y++;
            setWorldBlock(block, (Block) {BLOCK_AIR, 0});
            block->y--;
        }
    }
    //Remove other half of piston (yes, this is a special case)
    else if(toRemove->type == BLOCK_PISTON_BASE)
    {
        BlockPos pos = {block->chunk, block->x, block->y, block->z};
        getBlockPosByDirection(toRemove->data & BLOCK_DATA_DIRECTION, &pos);
        setWorldBlock(&pos, (Block) {BLOCK_AIR, 0});
    }
    else if(toRemove->type == BLOCK_PISTON_HEAD)
    {
        BlockPos pos = {block->chunk, block->x, block->y, block->z};
        getBlockPosByInverseDirection(toRemove->data & BLOCK_DATA_DIRECTION, &pos);
        setWorldBlock(&pos, (Block) {BLOCK_AIR, 0});
    }
    else if(toRemove->type == BLOCK_COMPUTER)
    {
        //Remove computer data
        free(getWorldChunk(block)->computers[GET_COMPUTER_INDEX(toRemove->data)]);
        getWorldChunk(block)->computers[GET_COMPUTER_INDEX(toRemove->data)] = NULL;
    }

    setWorldBlock(block, (Block) {BLOCK_AIR, 0});
}

void calcFrameGame(Player* player, State* state, uint32_t ticks, bool invertY)
{
    //Player movement + look
    int8_t dirF = 0;
    if(keyPressed(B_X))
    {
        dirF = 1;
    }
    playerMove(player, dirF, ticks);

    int8_t dirX = 0;
    int8_t dirY = 0;
    if(keyPressed(B_UP))
    {
        dirX = 1;
    }
    else if(keyPressed(B_DOWN))
    {
        dirX = -1;
    }
    if(keyPressed(B_LEFT))
    {
        dirY = -1;
    }
    else if(keyPressed(B_RIGHT))
    {
        dirY = 1;
    }
    if(!invertY)
    {
        dirX *= -1;
    }
    playerLook(player, dirX, dirY, ticks);

    if(keyDown(B_Y) && !player->jumping)
    {
        player->jumping = JUMP_TIME;
    }

    calcPlayer(player, ticks);

    //Cast ray
    vec3 rayDir = anglesToDirection(&player->rotation);
    //Player position in world space
    vec3 posWorld = addv3(player->position, (vec3) {VIEW_TRANSLATION, VIEW_TRANSLATION, VIEW_TRANSLATION});

    BlockPos block;
    float distance;
    AABBSide result = intersectsRayWorld(&posWorld, &rayDir, &block, &distance);

    if(keyUp(B_A) && result)
    {
        tryPlaceBlockOrInteract(player, state, &block, result);
    }
    //Remove block
    else if(keyUp(B_B) && result)
    {
        tryRemoveBlock(&block);
    }

    if(keyUp(B_SELECT))
    {
        scrollHotbar();
    }
    else if(keyUp(B_START))
    {
        *state = STATE_INVENTORY;
    }
    else if(keyUp(B_MENU))
    {
        *state = STATE_MENU;
    }

    calcWorld(&player->position, ticks);
}

void precalcGame(Player* player, uint32_t ticks)
{
    for(uint8_t i = 0; i < (VIEW_DISTANCE * VIEW_DISTANCE * VIEW_DISTANCE) / MAX_CHUNKS_PER_FRAME; i++)
    {
        calcWorld(&player->position, ticks);
    }
}

void newGame(Player* player, uint32_t newGameSeed)
{
    quitWorld();
    initWorld(newGameSeed);
    player->position = (vec3) {0, 0, 0};
    player->rotation = (vec3) {0, 0, 0};
    resetHotbar();
    precalcGame(player, 1);
}

void calcFrameInventory(State* state)
{
    int8_t dirX = 0;
    int8_t dirY = 0;
    int8_t dirTab = 0;
    if(keyUp(B_UP))
    {
        dirY = -1;
    }
    else if(keyUp(B_DOWN))
    {
        dirY = 1;
    }
    if(keyUp(B_LEFT))
    {
        dirX = -1;
    }
    else if(keyUp(B_RIGHT))
    {
        dirX = 1;
    }
    if(keyUp(B_TL) || keyUp(B_X))
    {
        dirTab = -1;
    }
    else if(keyUp(B_TR) || keyUp(B_Y))
    {
        dirTab = 1;
    }
    scrollInventory(dirX, dirY, dirTab);

    if(keyUp(B_A))
    {
        selectInventorySlot();
    }

    if(keyUp(B_SELECT))
    {
        scrollHotbar();
    }
    else if(keyUp(B_START))
    {
        *state = STATE_GAME;
    }
}

void calcFrameProgramming(Player* player, State* state, uint32_t ticks)
{
    calcWorld(&player->position, ticks);

    int8_t dirX = 0;
    int8_t dirY = 0;
    if(keyUp(B_UP))
    {
        dirY = -1;
    }
    else if(keyUp(B_DOWN))
    {
        dirY = 1;
    }
    if(keyUp(B_LEFT))
    {
        dirX = -1;
    }
    else if(keyUp(B_RIGHT))
    {
        dirX = 1;
    }
    moveProgrammingCursor(dirX, dirY);

    if(keyUp(B_A))
    {
        enterProgrammingCursor(programmingComputer);
    }
    else if(keyUp(B_B))
    {
        if(!cancelProgrammingCursor())
        {
            *state = STATE_GAME;
        }
    }
    else if(keyUp(B_X))
    {
        shiftProgramDown(programmingComputer);
    }
    else if(keyUp(B_Y))
    {
        shiftProgramUp(programmingComputer);
    }
    else if(keyUp(B_START))
    {
        if(programmingComputer->af & COMPUTER_FLAG_RUNNING)
        {
            programmingComputer->af &= ~COMPUTER_FLAG_RUNNING;
        }
        else
        {
            programmingComputer->af |= COMPUTER_FLAG_RUNNING;
        }
    }
}