#include "aabb.h"

bool aabbIntersectsPoint(AABB* box, vec3* point)
{
    return (point->x >= box->min.x && point->x <= box->max.x) &&
            (point->y >= box->min.y && point->y <= box->max.y) &&
            (point->z >= box->min.z && point->z <= box->max.z);
}

bool aabbIntersectsAABB(AABB* box1, AABB* box2)
{
    return (box1->min.x <= box2->max.x && box1->max.x >= box2->min.x) &&
            (box1->min.y <= box2->max.y && box1->max.y >= box2->min.y) &&
            (box1->min.z <= box2->max.z && box1->max.z >= box2->min.z);
}

bool aabbInsideAABB(AABB* inner, AABB* outer)
{
    return (outer->min.x <= inner->min.x && outer->max.x >= inner->max.x) &&
            (outer->min.y <= inner->min.y && outer->max.y >= inner->max.y) &&
            (outer->min.z <= inner->min.z && outer->max.z >= inner->max.z);
}

//Based on this: https://gdbooks.gitbooks.io/3dcollisions/content/Chapter3/raycast_aabb.html
AABBSide aabbIntersectsRay(AABB* box, vec3* origin, vec3* direction, float* distance)
{
    vec3 t1 = {
        (box->min.x - origin->x) / direction->x,
        (box->min.y - origin->y) / direction->y,
        (box->min.z - origin->z) / direction->z
    };
    vec3 t2 = {
        (box->max.x - origin->x) / direction->x,
        (box->max.y - origin->y) / direction->y,
        (box->max.z - origin->z) / direction->z
    };

    float tMin = fmax(fmax(fmin(t1.x, t2.x), fmin(t1.y, t2.y)), fmin(t1.z, t2.z));
    float tMax = fmin(fmin(fmax(t1.x, t2.x), fmax(t1.y, t2.y)), fmax(t1.z, t2.z));

    //Ray intersects AABB behind us || ray doesn't intersect
    if(tMax < 0 || tMin > tMax)
    {
        return AABB_NONE;
    }

    if(tMin < 0)
    {
        *distance = tMax;
    }
    else
    {
        *distance = tMin;
    }

    if(tMin == t1.x || tMin == t2.x)
        return direction->x < 0 ? AABB_RIGHT : AABB_LEFT;

    if(tMin == t1.y || tMin == t2.y)
        return direction->y < 0 ? AABB_TOP : AABB_BOTTOM;

    //if(tMin == t1.z || tMin == t2.z)
        return direction->z < 0 ? AABB_BACK : AABB_FRONT;
}

void moveAABB(AABB* aabb, vec3 offset)
{
    aabb->min = addv3(aabb->min, offset);
    aabb->max = addv3(aabb->max, offset);
}

#ifdef DEBUG
void printAABB(AABB* aabb)
{
    printf("AABB{.min={%f, %f, %f}, .max={%f, %f, %f}}\n", aabb->min.x, aabb->min.y, aabb->min.z, aabb->max.x, aabb->max.y, aabb->max.z);
}
#endif