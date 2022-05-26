#ifndef _PLAYER_H
#define _PLAYER_H

#include <stdint.h>

#include "engine/includes/3dMath.h"
#include "aabb.h"

typedef struct {
    vec3 position;
    vec3 rotation;

    float speed;
    float turnSpeedX;
    float turnSpeedY;
    uint8_t jumping;

    AABB aabb;
} Player;

#define PLAYER_WIDTH  0.8f
#define PLAYER_HEIGHT 1.8f

#define ACCELERATION 0.011f
#define MAX_TURN_SPEED 4
#define MAX_SPEED 10

#define GRAVITY 9.8f

#define JUMP_TIME 150

void playerMove(Player* player, int8_t dir, uint32_t ticks);
void playerLook(Player* player, int8_t dirX, int8_t dirY, uint32_t ticks);

uint8_t playerIntersectsWorld(Player* player);
void calcPlayer(Player* player, uint32_t ticks);

void savePlayer(Player* player);
void loadPlayer(Player* player);

#endif