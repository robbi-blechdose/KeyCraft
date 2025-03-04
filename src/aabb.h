#ifndef AABB_H
#define AABB_H

#include <stdbool.h>

#include "fk-engine-core/includes/3dMath.h"

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
bool aabbInsideAABB(AABB* inner, AABB* outer);
AABBSide aabbIntersectsRay(AABB* box, vec3* origin, vec3* direction, float* distance);

void moveAABB(AABB* aabb, vec3 offset);

#ifdef DEBUG
void printAABB(AABB* aabb);
#endif

#endif