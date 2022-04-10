#ifndef _VIDEO_H
#define _VIDEO_H

#include "includes/3dMath.h"

#define WINX 240
#define WINY 240

void initVideo(vec4 clearColor, vec4 viewport, float fov, float near, float far);
void quitVideo();

void clearFrame();
void flipFrame();

#endif