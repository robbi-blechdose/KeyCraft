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
} Player;

void playerMove(Player* player, int8_t dir, uint32_t ticks);
void playerLook(Player* player, int8_t dirX, int8_t dirY, uint32_t ticks);
void calcPlayer(Player* player, uint32_t ticks);

#endif