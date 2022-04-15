#ifndef _AABB_H
#define _AABB_H

#include <stdint.h>

#include "engine/includes/3dMath.h"

typedef struct {
    vec3 min;
    vec3 max;
} AABB;

uint8_t aabbIntersectsPoint(AABB* box, vec3* point);
uint8_t aabbIntersectsAABB(AABB* box1, AABB* box2);
uint8_t aabbIntersectsRay(AABB* box, vec3* origin, vec3* direction, float* hit);

#endif