#ifndef _AABB_H
#define _AABB_H

#include <stdbool.h>

#include "engine/includes/3dMath.h"

typedef struct {
    vec3 min;
    vec3 max;
} AABB;

typedef enum {
    AABB_NONE = 0,
    AABB_FRONT,
    AABB_BACK,
    AABB_LEFT,
    AABB_RIGHT,
    AABB_TOP,
    AABB_BOTTOM
} AABBSide;

bool aabbIntersectsPoint(AABB* box, vec3* point);
bool aabbIntersectsAABB(AABB* box1, AABB* box2);
AABBSide aabbIntersectsRay(AABB* box, vec3* origin, vec3* direction, float* distance);

#endif