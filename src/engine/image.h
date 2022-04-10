#ifndef _IMAGE_H
#define _IMAGE_H

#include "GL/gl.h"
#include <SDL/SDL.h>

/**
 * Pixel To Coordinate
 * Converts a pixel position (0-255) to a texture coordinate (0-1)
 **/
#define PTC(X) ((X) / 256.0f)

void initPNG();
SDL_Surface* loadPNG(const char* path);
GLuint loadRGBTexture(unsigned char* path);
void deleteRGBTexture(GLuint texture);

void blitSurface(SDL_Surface* target, SDL_Surface* source, uint8_t x, uint8_t y);

#endif