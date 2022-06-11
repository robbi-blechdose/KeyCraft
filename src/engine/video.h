#ifndef _VIDEO_H
#define _VIDEO_H

#include "includes/3dMath.h"

#define WINX 240
#define WINY 240

#define TEXT_WHITE  0xFFFFFF
#define TEXT_YELLOW 0xFFFF00

void initVideo(vec4 clearColor, vec4 viewport, float fov, float near, float far);
void quitVideo();

void clearFrame();
void flipFrame();

void setOrtho();
void setPerspective();

#endif