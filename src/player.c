#include "player.h"

#include "engine/util.h"
#include "world.h"
#include "chunk.h"

#define ACCELERATION (1.0f / 75.0f)
#define MAX_TURN_SPEED 4
#define MAX_SPEED 10

void calcAcceleration(float* var, int8_t dir, uint32_t ticks)
{
    if(dir != 0)
    {
        *var += (dir * (float) ticks) * ACCELERATION;
    }
    else
    {
        if(*var < 0.1f && *var > -0.1f)
        {
            *var = 0;
        }
        else if(*var > 0)
        {
            *var -= ticks * ACCELERATION;
        }
        else if(*var < 0)
        {
            *var += ticks * ACCELERATION;
        }
    }
}

void playerMove(Player* player, int8_t dir, uint32_t ticks)
{
    player->speed = MAX_SPEED * dir;
}

void playerLook(Player* player, int8_t dirX, int8_t dirY, uint32_t ticks)
{
    calcAcceleration(&player->turnSpeedX, dirX, ticks);
    calcAcceleration(&player->turnSpeedY, dirY, ticks);
    player->turnSpeedX = clampf(player->turnSpeedX, -MAX_TURN_SPEED, MAX_TURN_SPEED);
    player->turnSpeedY = clampf(player->turnSpeedY, -MAX_TURN_SPEED, MAX_TURN_SPEED);
}

void calcPlayerAABB(Player* player)
{
    player->aabb.min = (vec3) {.x = player->position.x - PLAYER_WIDTH / 2 + 20,
                                .y = player->position.y - PLAYER_HEIGHT + 20,
                                .z = player->position.z - PLAYER_WIDTH / 2 + 20};
    player->aabb.max = (vec3) {.x = player->position.x + PLAYER_WIDTH / 2 + 20,
                                .y = player->position.y + 20,
                                .z = player->position.z + PLAYER_WIDTH / 2 + 20};
}

uint8_t getValueWithCollision(Player* player)
{
    calcPlayerAABB(player);
    return intersectsAABBWorld(&player->aabb);
    //return intersectsAABBChunk(getPlayerChunk(&player->position), &player->aabb);
}

void calcPlayer(Player* player, uint32_t ticks)
{
    //Apply turn speed
    player->rotation.x += (player->turnSpeedX * ticks) / 1000.0f;
    player->rotation.y += (player->turnSpeedY * ticks) / 1000.0f;
    //Keep rotation in bounds
    player->rotation.x = clampf(player->rotation.x, -M_PI_2, M_PI_2);
    clampAngle(&player->rotation.y);

    //Apply movement to position
    float diff = (player->speed * ticks) / 1000.0f;
    /**
    player->position.z -= cos(player->rotation.y) * cos(player->rotation.x) * diff;
    player->position.x += sin(player->rotation.y) * cos(player->rotation.x) * diff;
    player->position.y -= sin(player->rotation.x) * diff;
    **/
   
    float old = player->position.x;
    player->position.x += sin(player->rotation.y) * diff;
    if(getValueWithCollision(player))
    {
        player->position.x = old;
    }

    old = player->position.z;
    player->position.z -= cos(player->rotation.y) * diff;
    if(getValueWithCollision(player))
    {
        player->position.z = old;
    }

    //Apply gravity - or jumping
    old = player->position.y;
    if(player->jumping)
    {
        if(player->jumping >= ticks)
        {
            player->jumping -= ticks;
        }
        else
        {
            player->jumping = 0;
        }
        player->position.y += GRAVITY * ticks / 1000.0f;
    }
    else
    {
        player->position.y -= GRAVITY * ticks / 1000.0f;
    }
    //Collision check for y direction (don't move if that would cause us to collide)
    if(getValueWithCollision(player))
    {
        player->position.y = old;
    }

    //TODO: Collision check
    //Idea: collide player aabb against chunk 2,2 aabb (chunk 2,2 because the player is always in this chunk)
    
    //printf("Pos: %f %f %f\n", player->position.x, player->position.y, player->position.z);
    //printf("Rot: %f %f %f\n", player->rotation.x, player->rotation.y, player->rotation.z);
}