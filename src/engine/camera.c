#include "camera.h"

#include "GL/gl.h"

vec3 cameraPos;
//x = pitch, y = yaw, z = roll
vec3 cameraRot;

void drawCamera(vec3* pos, vec3* rot)
{
	glLoadIdentity();
    glRotatef(RAD_TO_DEG(rot->x), 1, 0, 0);
    glRotatef(RAD_TO_DEG(rot->y), 0, 1, 0);
    glRotatef(RAD_TO_DEG(rot->z), 0, 0, 1);
    glTranslatef(-pos->x, -pos->y, -pos->z);
}

void drawCameraPrep()
{
    drawCamera(&cameraPos, &cameraRot);
}

void setCameraPos(vec3 pos)
{
    cameraPos = pos;
}

void setCameraRot(vec3 rot)
{
    cameraRot = rot;
}