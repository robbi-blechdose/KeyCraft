#include "player.h"

#include "fk-engine-core/util.h"
#include "fk-engine-core/savegame.h"

#include "world.h"
#include "chunk.h"

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
    player->aabb.min = (vec3) {.x = player->position.x - PLAYER_WIDTH / 2 + VIEW_TRANSLATION,
                                .y = player->position.y - PLAYER_HEIGHT + VIEW_TRANSLATION,
                                .z = player->position.z - PLAYER_WIDTH / 2 + VIEW_TRANSLATION};
    player->aabb.max = (vec3) {.x = player->position.x + PLAYER_WIDTH / 2 + VIEW_TRANSLATION,
                                .y = player->position.y + VIEW_TRANSLATION,
                                .z = player->position.z + PLAYER_WIDTH / 2 + VIEW_TRANSLATION};
}

bool playerIntersectsWorld(Player* player)
{
    calcPlayerAABB(player);
    return intersectsAABBWorld(&player->aabb);
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

    float old = player->position.x;
    player->position.x += sin(player->rotation.y) * diff;
    if(playerIntersectsWorld(player))
    {
        player->position.x = old;
    }

    old = player->position.z;
    player->position.z -= cos(player->rotation.y) * diff;
    if(playerIntersectsWorld(player))
    {
        player->position.z = old;
    }

    //Apply gravity - or jumping
    old = player->position.y;
    if(player->jumping > 1)
    {
        if(player->jumping > ticks)
        {
            player->jumping -= ticks;
        }
        else
        {
            player->jumping = 1;
        }
        player->position.y += GRAVITY * ticks / 1000.0f;
    }
    else
    {
        player->position.y -= GRAVITY * ticks / 1000.0f;
    }
    //Collision check for y direction (don't move if that would cause us to collide)
    if(playerIntersectsWorld(player))
    {
        player->position.y = old;
        if(player->jumping)
        {
            player->jumping = 0;
        }
    }
}

void savePlayer(Player* player)
{
    writeElement(&player->position, sizeof(vec3));
    writeElement(&player->rotation, sizeof(vec3));
}

void loadPlayer(Player* player)
{
    readElement(&player->position, sizeof(vec3));
    readElement(&player->rotation, sizeof(vec3));
    player->position.y += 0.1f; //Safety buffer to make sure we don't accidentally glitch into a block
}