#include "player.h"

#include "engine/util.h"

#define ACCELERATION (1.0f / 125.0f)
#define MAX_TURN_SPEED 5
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
    //calcAcceleration(&player->speed, dir, ticks);
    //player->speed = clampf(player->speed, 0, MAX_SPEED);
    player->speed = MAX_SPEED * dir;
}

void playerLook(Player* player, int8_t dirX, int8_t dirY, uint32_t ticks)
{
    calcAcceleration(&player->turnSpeedX, dirX, ticks);
    calcAcceleration(&player->turnSpeedY, dirY, ticks);
    player->turnSpeedX = clampf(player->turnSpeedX, -MAX_TURN_SPEED, MAX_TURN_SPEED);
    player->turnSpeedY = clampf(player->turnSpeedY, -MAX_TURN_SPEED, MAX_TURN_SPEED);
}

void calcPlayer(Player* player, uint32_t ticks)
{
    //Apply turn speed
    player->rotation.x += (player->turnSpeedX * ticks) / 1000.0f;
    player->rotation.y += (player->turnSpeedY * ticks) / 1000.0f;
    //Keep rotation in bounds
    clampAngle(&player->rotation.x);
    clampAngle(&player->rotation.y);

    //Apply movement to position
    float diff = (player->speed * ticks) / 1000.0f;
    player->position.z -= cos(player->rotation.y) * cos(player->rotation.x) * diff;
    player->position.x += sin(player->rotation.y) * cos(player->rotation.x) * diff;
    player->position.y -= sin(player->rotation.x) * diff;
    
    //printf("Pos: %f %f %f\n", player->position.x, player->position.y, player->position.z);
    //printf("Rot: %f %f %f\n", player->rotation.x, player->rotation.y, player->rotation.z);
}