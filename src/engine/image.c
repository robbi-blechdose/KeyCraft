#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "includes/stb_image.h"
#include <SDL_image.h>

void initPNG()
{
    IMG_Init(IMG_INIT_PNG);
}

SDL_Surface* loadPNG(const char* path)
{
    SDL_Surface* loaded = IMG_Load(path);
    if(loaded != NULL)
    {
        SDL_Surface* converted = SDL_DisplayFormatAlpha(loaded);
        SDL_FreeSurface(loaded);
        if(converted != NULL)
        {
            return converted;
        }
    }
    printf("PNG loading failed for \"%s\".\n", path);
    return NULL;
}

GLuint loadRGBTexture(unsigned char* path)
{
    int sw = 0, sh = 0, sc = 0; // sc goes unused.
    unsigned char* sourceData = stbi_load(path, &sw, &sh, &sc, 3);
	GLuint t = 0;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, sw, sh, 0, GL_RGB, GL_UNSIGNED_BYTE, sourceData);
	free(sourceData);
	return t;
}

void deleteRGBTexture(GLuint texture)
{
    glDeleteTextures(1, &texture);
}

void drawTexQuad(float posX, float posY, float sizeX, float sizeY, float z,
					float texX1, float texY1, float texX2, float texY2)
{
    glTexCoord2f(texX1, texY2);
    glVertex3f(posX, posY, z);
    glTexCoord2f(texX2, texY2);
    glVertex3f(posX + sizeX - 1, posY, z);
    glTexCoord2f(texX2, texY1);
    glVertex3f(posX + sizeX - 1, posY + sizeY - 1, z);
    glTexCoord2f(texX1, texY1);
    glVertex3f(posX, posY + sizeY - 1, z);
}