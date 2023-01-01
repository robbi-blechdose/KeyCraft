#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

#include "includes/3dMath.h"

#define WINX 240
#define WINY 240

#define TEXT_BLACK  0x000000
#define TEXT_WHITE  0xFFFFFF
#define TEXT_YELLOW 0xFFFF00

void initVideo(vec4 clearColor, vec4 viewport, float fov, float near, float far);
void quitVideo();

void clearFrame();
void flipFrame();

void setOrtho();
void setPerspective();

void drawFPS(uint16_t fps);

#endif