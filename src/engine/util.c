#include "util.h"
#include <stdlib.h>

float squaref(float a)
{
    return a * a;
}

float distance3d(vec3* a, vec3* b)
{
    return sqrtf(squaref(b->x - a->x) + squaref(b->y - a->y) + squaref(b->z - a->z));
}

float distance2d(vec2* a, vec2* b)
{
    return sqrtf(squaref(b->x - a->x) + squaref(b->y - a->y));
}

void clampAngle(float* angle)
{
    if(*angle < 0)
    {
        *angle += 2 * M_PI;
    }
    if(*angle > 2 * M_PI)
    {
        *angle -= 2 * M_PI;
    }
}

uint32_t randr(uint32_t max)
{
    return rand() / (RAND_MAX / (max + 1));
}

float randf(float max)
{
    return ((float)rand() / (float)(RAND_MAX)) * max;
}

//Möller-Trumbore algorithm
int intersectTriangle(vec3 origin, vec3 direction, vec3 vert0, vec3 vert1, vec3 vert2,
                      float* t, float* u, float* v)
{
    vec3 edge1, edge2, tvec, pvec, qvec;
    float det, invDet;

    //Find vectors for two edges sharing vert0
    edge1 = subv3(vert1, vert0);
    edge2 = subv3(vert2, vert0);

    //Begin calculating determinant
    pvec = crossv3(direction, edge2);

    //If determinant is near 0, the ray is in the triangle plane
    det = dotv3(edge1, pvec);

    //Cull variant for extra speed
    if(det < EPSILON)
    {
        return 0;
    }

    //Calculate distance from vert0 to ray origin
    tvec = subv3(origin, vert0);

    //Calculate u parameter and test bounds
    *u = dotv3(tvec, pvec);
    if(*u < 0.0f || *u > det)
    {
        return 0;
    }

    //Prepare to test v parameter
    qvec = crossv3(tvec, edge1);

    //Calculate v parameter and test bounds
    *v = dotv3(direction, qvec);
    if(*v < 0.0f || *u + *v > det)
    {
        return 0;
    }

    //Calculate t, scale parameters, ray intersects triangle
    *t = dotv3(edge2, qvec);
    invDet = 1.0f / det;
    *t *= invDet;
    *u *= invDet;
    *v *= invDet;

    return 1;
}

void calcRotToTarget(vec3* pos, vec3* target, float* yRot, float* xRot)
{
    vec3 diff = subv3(*pos, *target);
    diff = normalizev3(diff);

    *yRot = atan2f(diff.z, diff.x) - M_PI_2;
    clampAngle(yRot);
    *xRot = asinf(diff.y);
}

vec3 anglesToDirection(vec3* rotation)
{
    vec3 result = {.x = 0, .y = 0, .z = 1};
    vec3 dir = {.x = 1, .y = 0, .z = 0};
    result = rotatev3(result, dir, rotation->x);
    dir = (vec3) {.x = 0, .y = 1, .z = 0};
    result = rotatev3(result, dir, M_PI - rotation->y);
    return result;
}

float checkHitSphere(vec3* position, vec3* direction, vec3* center, float radius)
{
    //OC = ray origin to sphere center
    vec3 oc = subv3(*position,*center);
    
    //TODO: Test
    float b = dotv3(oc, *direction);
    float c = dotv3(oc, oc) - (radius * radius);
    if(!(c > 0.0f && b > 0.0f))
    {
        float discr = b * b - c;
        if(discr >= 0)
        {
            return -b;
        }
    }
    return -1;

    //TODO: Test alternative version
    /**
    float a = dotv3(rot, rot);
    float b = 2 * dotv3(oc, rot);
    float c = dotv3(oc, oc) - (radius * radius);
    float discriminant = b * b - 4 * a * c;
    if(discriminant >= 0)
    {
        return (-b + sqrt(b * b - 4 * a * c)) / 2 * a;
    }
    return -1;
    **/
}