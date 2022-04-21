#include <SDL/SDL.h>
#include <GL/gl.h>

#include "engine/video.h"
#include "engine/audio.h"
#include "engine/input.h"
#include "engine/camera.h"

#include "player.h"
#include "world.h"
#include "blockutils.h"
#include "block.h"
#include "inventory.h"

#define DEBUG

#define MAX_FPS 50
//#define LIMIT_FPS

#ifdef DEBUG
uint16_t fps;
#endif

//---------- Main game stuff ----------//
uint8_t running = 1;

Player player;

#ifdef DEBUG
void drawFPS(uint16_t fps)
{
    char buffer[12];
	sprintf(buffer, "FPS: %i", fps);
	glDrawText(buffer, 2, 2, 0xFFFFFF);
}
#endif

void calcFrame(uint32_t ticks)
{
    //Player movement + look
    int8_t dirF = 0;
    if(keyPressed(B_X))
    {
        dirF = 1;
    }
    playerMove(&player, dirF, ticks);
    int8_t dirX = 0;
    int8_t dirY = 0;
    if(keyPressed(B_UP))
    {
        dirX = 1;
    }
    else if(keyPressed(B_DOWN))
    {
        dirX = -1;
    }
    if(keyPressed(B_LEFT))
    {
        dirY = -1;
    }
    else if(keyPressed(B_RIGHT))
    {
        dirY = 1;
    }
    playerLook(&player, dirX, dirY, ticks);
    calcPlayer(&player, ticks);

    //Cast ray
    vec3 rayDir = anglesToDirection(&player.rotation);
    //Player position in world space
    vec3 posWorld = player.position;
    posWorld.x += 20;
    posWorld.y += 20;
    posWorld.z += 20;

    BlockPos block;
    float distance;
    AABBSide result = intersectsRayWorld(&posWorld, &rayDir, &block, &distance);

    //Place block
    if(keyUp(B_A) && result)
    {
        //Calc position
        switch(result)
        {
            case AABB_FRONT:
            {
                block.z -= BLOCK_SIZE;
                break;
            }
            case AABB_BACK:
            {
                block.z += BLOCK_SIZE;
                break;
            }
            case AABB_LEFT:
            {
                block.x -= BLOCK_SIZE;
                break;
            }
            case AABB_RIGHT:
            {
                block.x += BLOCK_SIZE;
                break;
            }
            case AABB_BOTTOM:
            {
                block.y -= BLOCK_SIZE;
                break;
            }
            case AABB_TOP:
            {
                block.y += BLOCK_SIZE;
                break;
            }
        }

        //Place new block
        if(getWorldBlock(&block)->type == BLOCK_AIR)
        {
            setWorldBlock(&block, (Block) {getHotbarSelection(), 0});
        }
    }
    //Remove block
    else if(keyUp(B_B) && result)
    {
        if(getWorldBlock(&block)->type != BLOCK_BEDROCK)
        {
            setWorldBlock(&block, (Block) {BLOCK_AIR, 0});
        }
    }

    if(keyUp(B_SELECT))
    {
        scrollHotbar();
    }

    calcWorld(&player.position, ticks);
}

void drawFrame()
{
    clearFrame();

    //3d drawing
    setPerspective();

    drawCamera(&player.position, &player.rotation);

    drawWorld();

    //GUI drawing
    setOrtho();
    glLoadIdentity();
    glBegin(GL_QUADS);
    drawHotbar();
    glEnd();

    #ifdef DEBUG
    drawFPS(fps);
    #endif

    flipFrame();
}

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    initVideo((vec4) {.d = {0, 0.8f, 1.0f, 1.0f}}, (vec4) {.d = {0, 0, WINX, WINY}}, 70, 0.5f, 256);
    initAudio(MIX_MAX_VOLUME, 2, 2);

    player.position = (vec3) {0, -5.0f, 0};

    initWorld();

    //Run main loop
	uint32_t tNow = SDL_GetTicks();
	uint32_t tLastFrame = tNow;
    uint16_t ticks = 0;
    while(running)
    {
		tNow = SDL_GetTicks();
        ticks = tNow - tLastFrame;
        
        running = handleInput();

        calcFrame(ticks);
        drawFrame();

        #ifdef LIMIT_FPS
		while((1000 / MAX_FPS) > (SDL_GetTicks() - tNow + 1))
        {
			SDL_Delay(1); //Yay stable framerate!
		}
        #endif
        #ifdef DEBUG
		fps = 1000.0f / (float)(tNow - tLastFrame);
        #endif
		tLastFrame = tNow;
    }

    //Cleanup
    quitAudio();
	quitVideo();

	return 0;
}