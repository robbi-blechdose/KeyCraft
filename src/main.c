#include <SDL/SDL.h>
#include <GL/gl.h>

#include "engine/video.h"
#include "engine/audio.h"
#include "engine/input.h"
#include "engine/camera.h"
#include "engine/image.h"

#include "player.h"
#include "world.h"
#include "blocks/blockutils.h"
#include "blocks/block.h"
#include "inventory.h"

#define DEBUG

#define MAX_FPS 50
//#define LIMIT_FPS

#ifdef DEBUG
uint16_t fps;
#endif

typedef enum {
    STATE_GAME,
    STATE_INVENTORY//,
    //STATE_OPTIONS //TODO
} State;

//---------- Main game stuff ----------//
uint8_t running = 1;
State state = STATE_GAME;
Player player;

#ifdef DEBUG
void drawFPS(uint16_t fps)
{
    char buffer[12];
	sprintf(buffer, "FPS: %i", fps);
	glDrawText(buffer, 2, 2, 0xFFFFFF);
}
#endif

void calcFrameGame(uint32_t ticks)
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
    if(keyUp(B_Y) && !player.jumping)
    {
        player.jumping = JUMP_TIME;
    }
    calcPlayer(&player, ticks);

    //Cast ray
    vec3 rayDir = anglesToDirection(&player.rotation);
    //Player position in world space
    vec3 posWorld = player.position;
    posWorld.x += VIEW_TRANSLATION;
    posWorld.y += VIEW_TRANSLATION;
    posWorld.z += VIEW_TRANSLATION;

    BlockPos block;
    float distance;
    AABBSide result = intersectsRayWorld(&posWorld, &rayDir, &block, &distance);

    //Place block
    if(keyUp(B_A) && result)
    {
        uint8_t canPlace = !actWorldBlock(&block);

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
        if(getWorldBlock(&block)->type == BLOCK_AIR && canPlace)
        {
            BlockPos below = block;
            below.y -= BLOCK_SIZE;
            BlockPos above = block;
            above.y += BLOCK_SIZE;
            if(canPlaceBlock(getHotbarSelection(), getWorldBlock(&below)->type) &&
                !(getHotbarSelection() == BLOCK_DOOR && getWorldBlock(&above)->type != BLOCK_AIR)) //Check if we can place the door upper (yes, this is a special case)
            {
                uint8_t blockData = 0;

                if(isBlockOriented(getHotbarSelection()))
                {
                    uint8_t orientation = BLOCK_DATA_DIR_RIGHT;
                    float rotation = player.rotation.y - M_PI_4;
                    clampAngle(&rotation);

                    if(rotation < M_PI_2)
                    {
                        orientation = BLOCK_DATA_DIR_FRONT;
                    }
                    else if(rotation < M_PI)
                    {
                        orientation = BLOCK_DATA_DIR_LEFT;
                    }
                    else if(rotation < M_PI + M_PI_2)
                    {
                        orientation = BLOCK_DATA_DIR_BACK;
                    }
                    //4th case is covered by the original assignment
                    blockData += orientation;
                }

                setWorldBlock(&block, (Block) {getHotbarSelection(), blockData});
                //Check if the block intersects with the player. If so, don't place it
                if(playerIntersectsWorld(&player))
                {
                    setWorldBlock(&block, (Block) {BLOCK_AIR, 0});
                    //Remove door upper as well (yes, this is a special case)
                    if(getHotbarSelection() == BLOCK_DOOR)
                    {
                        setWorldBlock(&above, (Block) {BLOCK_AIR, 0});
                    }
                }
            }
        }
    }
    //Remove block
    else if(keyUp(B_B) && result)
    {
        if(getWorldBlock(&block)->type != BLOCK_BEDROCK)
        {
            //Remove other door half (yes, this is a special case)
            if(getWorldBlock(&block)->type == BLOCK_DOOR)
            {
                if(getWorldBlock(&block)->data & BLOCK_DATA_PART)
                {
                    //This is the upper part, remove lower
                    block.y--;
                    setWorldBlock(&block, (Block) {BLOCK_AIR, 0});
                    block.y++;
                }
                else
                {
                    //This is the lower part, remove upper
                    block.y++;
                    setWorldBlock(&block, (Block) {BLOCK_AIR, 0});
                    block.y--;
                }
            }

            setWorldBlock(&block, (Block) {BLOCK_AIR, 0});
        }
    }

    if(keyUp(B_SELECT))
    {
        scrollHotbar();
    }

    if(keyUp(B_START))
    {
        state = STATE_INVENTORY;
    }

    calcWorld(&player.position, ticks);
}

void calcFrame(uint32_t ticks)
{
    switch(state)
    {
        case STATE_GAME:
        {
            calcFrameGame(ticks);
            break;
        }
        case STATE_INVENTORY:
        {
            int8_t dirX = 0;
            int8_t dirY = 0;
            if(keyUp(B_UP))
            {
                dirY = -1;
            }
            else if(keyUp(B_DOWN))
            {
                dirY = 1;
            }
            if(keyUp(B_LEFT))
            {
                dirX = -1;
            }
            else if(keyUp(B_RIGHT))
            {
                dirX = 1;
            }
            scrollInventory(dirX, dirY);

            if(keyUp(B_A))
            {
                selectInventorySlot();
            }

            if(keyUp(B_SELECT))
            {
                scrollHotbar();
            }

            if(keyUp(B_START))
            {
                state = STATE_GAME;
            }
            break;
        }
    }
}

void drawFrame()
{
    clearFrame();

    //3d drawing
    setPerspective();
    drawCamera(&player.position, &player.rotation);

    drawWorld(&player.position, &player.rotation);

    //GUI drawing
    setOrtho();
    glLoadIdentity();
    glBegin(GL_QUADS);
    //Crosshair
    if(state == STATE_GAME)
    {
        drawTexQuad(WINX / 2 - 8, WINY / 2 - 8, 16, 16, 10, PTC(240), PTC(64), PTC(240 + 15), PTC(64 + 15));
    }
    else //if(state == STATE_INVENTORY
    {
        drawInventory();
    }
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
    initVideo((vec4) {.d = {0, 0.8f, 1.0f, 1.0f}}, (vec4) {.d = {0, 0, WINX, WINY}}, 70, 0.3f, 8 * VIEW_DISTANCE);
    initAudio(MIX_MAX_VOLUME, 2, 2);

    player.position = (vec3) {0, 0, 0};

    initWorld();

    loadWorld();

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

    saveWorld();

    //Cleanup
    quitWorld();
    
    quitAudio();
	quitVideo();

	return 0;
}