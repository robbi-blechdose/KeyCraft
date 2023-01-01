#ifndef CAMERA_H
#define CAMERA_H

#include "util.h"
#include "includes/3dMath.h"

void drawCamera(vec3* pos, vec3* rot);
void drawCameraPrep();
void setCameraPos(vec3 pos);
void setCameraRot(vec3 rot);

#endif