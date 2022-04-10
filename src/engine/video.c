#include "video.h"

#include <SDL/SDL.h>
#define CHAD_API_IMPL
#include "zbuffer.h"
#include "GL/gl.h"

SDL_Surface* screen;
ZBuffer* frameBuffer = NULL;

void initVideo(vec4 clearColor, vec4 viewport, float fov, float near, float far)
{
    screen = SDL_SetVideoMode(WINX, WINY, 16, SDL_SWSURFACE);
	SDL_ShowCursor(SDL_DISABLE);

    //Initialize TinyGL
	frameBuffer = ZB_open(WINX, WINY, ZB_MODE_5R6G5B, 0);
	glInit(frameBuffer);
	glShadeModel(GL_FLAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(clearColor.d[0], clearColor.d[1], clearColor.d[2], clearColor.d[3]);
	glClearDepth(1.0f);
    glViewport(viewport.d[0], viewport.d[1], viewport.d[2], viewport.d[3]);
	glTextSize(GL_TEXT_SIZE8x8);
	glEnable(GL_TEXTURE_2D);
	
	//Initialize projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    mat4 mPerspective = perspective(fov, (float) WINX / (float) WINY, near, far);
	glLoadMatrixf(mPerspective.d);
	glMatrixMode(GL_MODELVIEW);
}

void clearFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void flipFrame()
{
    if(SDL_MUSTLOCK(screen))
    {
        SDL_LockSurface(screen);
    }
    ZB_copyFrameBuffer(frameBuffer, screen->pixels, screen->pitch);
	if(SDL_MUSTLOCK(screen))
    {
		SDL_UnlockSurface(screen);
    }
	SDL_Flip(screen);
}

void quitVideo()
{
	ZB_close(frameBuffer);
	glClose();

    SDL_Quit();
}