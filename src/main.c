#include <SDL/SDL.h>
#include <GL/gl.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#include "engine/video.h"
#include "engine/audio.h"
#include "engine/input.h"
#include "engine/camera.h"
#include "engine/image.h"
#include "engine/savegame.h"

#include "player.h"
#include "world.h"
#include "blocks/blockutils.h"
#include "blocks/block.h"
#include "inventory.h"
#include "gui/menu.h"
#include "sfx.h"

#define DEBUG

#define MAX_FPS 50
//#define LIMIT_FPS

#ifdef DEBUG
uint16_t fps;
#define FPS_WINDOW 250
uint32_t counterFrames = 0;
uint32_t counterTime = 0;
#endif

typedef enum {
    STATE_GAME,
    STATE_INVENTORY,
    STATE_MENU
} State;

#define SAVE_VERSION 30
#define SAVE_NAME             "game.sav"
#define INSTANTPLAY_SAVE_NAME "instantplay.sav"

#define SHELL_CMD_POWERDOWN_HANDLE "powerdown handle"
#define SHELL_CMD_INSTANT_PLAY     "instant_play"

char* programName = NULL;

//---------- Main game stuff ----------//
bool running = true;
bool quickSaveAndPoweroff = false;
State state = STATE_MENU;
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
        bool canPlace = !actWorldBlock(&block);

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
        if(canPlace && getWorldBlock(&block)->type == BLOCK_AIR)
        {
            BlockPos below = block;
            below.y -= BLOCK_SIZE;
            BlockPos above = block;
            above.y += BLOCK_SIZE;
            Block toPlace = getHotbarSelection();
            if(canPlaceBlock(toPlace.type, getWorldBlock(&below)->type) &&
                !(toPlace.type == BLOCK_DOOR && getWorldBlock(&above)->type != BLOCK_AIR)) //Check if we can place the door upper (yes, this is a special case)
            {
                if(isBlockOriented(toPlace.type))
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
                    toPlace.data |= orientation;
                }

                setWorldBlock(&block, toPlace);
                //Check if the block intersects with the player. If so, don't place it
                if(playerIntersectsWorld(&player))
                {
                    setWorldBlock(&block, (Block) {BLOCK_AIR, 0});
                    //Remove door upper as well (yes, this is a special case)
                    if(toPlace.type == BLOCK_DOOR)
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
            int8_t dirTab = 0;
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
            if(keyUp(B_TL))
            {
                dirTab = -1;
            }
            else if(keyUp(B_TR))
            {
                dirTab = 1;
            }
            scrollInventory(dirX, dirY, dirTab);

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
        case STATE_MENU:
        {
            if(keyUp(B_UP))
            {
                scrollMenu(-1);
            }
            else if(keyUp(B_DOWN))
            {
                scrollMenu(1);
            }
            
            if(keyUp(B_START) || keyUp(B_A))
            {
                switch(getMenuCursor())
                {
                    case MENU_SELECTION_CONTINUE:
                    {
                        state = STATE_GAME;
                        break;
                    }
                    case MENU_SELECTION_NEW_GAME:
                    {
                        state = STATE_GAME;

                        //Destroy old game, initialize new one
                        quitWorld();
                        initWorld(0);
                        player.position = (vec3) {0, 0, 0};
                        player.rotation = (vec3) {0, 0, 0};
                        //TODO: reinit hotbar
                        //Run one frame to build geometry for the first time etc.
                        calcFrameGame(1);

                        break;
                    }
                    case MENU_SELECTION_OPTIONS:
                    {
                        //TODO
                        break;
                    }
                }
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
    if(state == STATE_GAME)
    {
        //Crosshair
        drawTexQuad(WINX / 2 - 8, WINY / 2 - 8, 16, 16, 20, PTC(240), PTC(64), PTC(240 + 15), PTC(64 + 15));
        drawHotbar();
    }
    else if(state == STATE_INVENTORY)
    {
        drawHotbar();
        drawInventory();
    }
    else //if(state == STATE_MENU)
    {
        drawMenu();
    }
    glEnd();

    #ifdef DEBUG
    drawFPS(fps);
    #endif

    flipFrame();
}

void saveGame(char* name)
{
    if(openSave(".keycraft", name, 1))
    {
        uint16_t saveVersion = SAVE_VERSION;
        writeElement(&saveVersion, sizeof(uint16_t));
        savePlayer(&player);
        saveHotbar();
        saveWorld();
        closeSave();
    }
}

void loadGame(char* name)
{
    if(openSave(".keycraft", name, 0))
    {
        uint16_t saveVersion;
        readElement(&saveVersion, sizeof(uint16_t));

        if(saveVersion / 10 == SAVE_VERSION / 10)
        {
            loadPlayer(&player);
            loadHotbar();
            loadWorld();
        }
        else
        {
            setMenuFlag(MENU_FLAG_LOADFAIL);
        }
        closeSave();
    }
    else
    {
        setMenuFlag(MENU_FLAG_NOSAVE);
    }
}

void handleSigusr1(int sig)
{
    running = false;
    quickSaveAndPoweroff = true;
}

int main(int argc, char **argv)
{
    //Save program name for instant play
    programName = argv[0];
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    initVideo((vec4) {.d = {0, 0.8f, 1.0f, 1.0f}}, (vec4) {.d = {0, 0, WINX, WINY}}, 70, 0.3f, 8 * VIEW_DISTANCE);
    initAudio(MIX_MAX_VOLUME, 1, 2);

    #ifdef FUNKEY
    loadMusic(0, "/opk/res/mus/curiouscritters.ogg");
    loadSampleIndex(SFX_LEVER, "/opk/res/sfx/click20.ogg");
    loadSampleIndex(SFX_TNT, "/opk/res/sfx/explosion.ogg");
    #else
    loadMusic(0, "res/mus/curiouscritters.ogg");
    loadSampleIndex(SFX_LEVER, "res/sfx/click20.ogg");
    loadSampleIndex(SFX_TNT, "res/sfx/explosion.ogg");
    #endif
    playMusic(0, 0);

    player.position = (vec3) {0, 0, 0};

    initWorld(0);

    if(argc > 1 && strcmp(argv[1], "-skipmenu") == 0)
    {
        state = STATE_GAME;
        loadGame(INSTANTPLAY_SAVE_NAME);
    }
    else
    {
        loadGame(SAVE_NAME);
    }

    //Run one frame to build geometry for the first time etc.
    calcFrameGame(1);

    //Register signal handler for SIGUSR1 (closing the console)
	signal(SIGUSR1, handleSigusr1);

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
		//fps = 1000.0f / (float)(tNow - tLastFrame);
        counterTime += ticks;
        counterFrames++;
        if(counterTime > FPS_WINDOW)
        {
            fps = ((float) counterFrames / counterTime) * 1000.0f;
            counterTime = 0;
            counterFrames = 0;
        }
        #endif

		tLastFrame = tNow;
    }

    if(quickSaveAndPoweroff)
    {
        //Console is closed, perform instant play save

        //Try to cancel shutdown
        FILE* fp = popen(SHELL_CMD_POWERDOWN_HANDLE, "r");
        if(fp != NULL)
        {
            pclose(fp);

            saveGame(INSTANTPLAY_SAVE_NAME);

            //Perform instant play save
            execlp(SHELL_CMD_INSTANT_PLAY, SHELL_CMD_INSTANT_PLAY, "save", programName, "-skipmenu", NULL);
        }
        //else: failed to cancel shutdown
    }
    else
    {
        //Normal exit, save game
        saveGame(SAVE_NAME);
    }

    //Cleanup
    quitWorld();
    
    quitAudio();
	quitVideo();

	return 0;
}