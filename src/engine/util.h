#ifndef UTIL_H
#define UTIL_H

#include <math.h>
#include <stdint.h>
#include "includes/3dMath.h"

#define RAD_TO_DEG(rad) ((rad) * 180.0f / M_PI)
#define DEG_TO_RAD(deg) ((deg) * M_PI / 180.0f)

typedef struct {
    float x;
    float y;
} vec2;

float squaref(float a);
float distance3d(vec3* a, vec3* b);
float distance2d(vec2* a, vec2* b);

void clampAngle(float* angle);

/**
 * Returns a random number in the range (0...max) (inclusive)
 **/
uint32_t randr(uint32_t max);

float randf(float max);

#define EPSILON 0.0001f
int intersectTriangle(vec3 origin, vec3 direction, vec3 vert0, vec3 vert1, vec3 vert2,
                      float* t, float* u, float* v);

void calcRotToTarget(vec3* pos, vec3* target, float* yRot, float* xRot);

vec3 anglesToDirection(vec3* rotation);
float checkHitSphere(vec3* position, vec3* direction, vec3* center, float radius);

//Resource path definition - instant play breaks relative paths, so the workaround with absolute paths is needed
#ifdef FUNKEY
#define RESOURCE(X) "/opk/" X
#else
#define RESOURCE(X) X
#endif

#endif