#ifndef IMAGE_H
#define IMAGE_H

#include "GL/gl.h"
#include <SDL/SDL.h>

/**
 * Pixel To Coordinate
 * Converts a pixel position (0-255) to a texture coordinate (0-1)
 **/
//Subtract 0.5 because OpenGL samples textures at the texel center, but we want the top-left corner (to get the entire pixel)
#define PTC(X) (((X) - 0.5f) / 255.0f)

void initPNG();
SDL_Surface* loadPNG(const char* path);
GLuint loadRGBTexture(unsigned char* path);
void deleteRGBTexture(GLuint texture);

void drawTexQuad(float posX, float posY, float sizeX, float sizeY, float z,
					float texX1, float texY1, float texX2, float texY2);

#endif