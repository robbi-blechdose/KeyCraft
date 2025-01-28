#include <SDL/SDL.h>
#include <GL/gl.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#include "fk-engine-core/video.h"
#include "fk-engine-core/audio.h"
#include "fk-engine-core/input.h"
#include "fk-engine-core/camera.h"
#include "fk-engine-core/image.h"
#include "fk-engine-core/savegame.h"

#include "player.h"
#include "world.h"
#include "blocks/blockutils.h"
#include "blocks/block.h"
#include "inventory.h"
#include "gui/menu.h"
#include "sfx.h"
#include "gui/programming.h"
#include "gameloop.h"

#include "saves.h"

#define MAX_FPS 50

#ifdef DEBUG
uint16_t fps;
#define FPS_WINDOW 250
uint32_t counterFrames = 0;
uint32_t counterTime = 0;
#endif

#define SHELL_CMD_POWERDOWN_HANDLE "powerdown handle"
#define SHELL_CMD_INSTANT_PLAY     "instant_play"

char* programName = NULL;

//---------- Main game stuff ----------//
bool running = true;
bool quickSaveAndPoweroff = false;
State state = STATE_MENU;
Player player;

//Options
bool invertY = true;
//TODO: each save will have to save its own seed
uint32_t newGameSeed = 0;
uint8_t gameIndex = 0;

//TODO: implement handling for load failures
//TODO: implement "Continue" menu option having the save number ("Continue Game 02")

void saveOptions()
{
    if(openSave(SAVE_FOLDER, OPTIONS_SAVE_NAME, true))
    {
        writeElement(&invertY, sizeof(bool));
        writeElement(&newGameSeed, sizeof(uint32_t));
        closeSave();
    }
}

void loadOptions()
{
    if(openSave(SAVE_FOLDER, OPTIONS_SAVE_NAME, false))
    {
        readElement(&invertY, sizeof(bool));
        readElement(&newGameSeed, sizeof(uint32_t));
        closeSave();
    }
}

void saveGame(char* name)
{
    //Skip saving if the world is unmodified
    //That means we lose the hotbar and player position, but that's a sacrifice I'm willing to make
    if(isWorldUnmodified())
    {
        return;
    }

    if(openSave(SAVE_FOLDER, name, true))
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
    if(openSave(SAVE_FOLDER, name, false))
    {
        SaveVersionCompat svc = readSaveVersionCompat();
        if(svc == SV_COMPAT_NONE)
        {
            setMenuFlag(MENU_FLAG_LOADFAIL);
            closeSave();
            return;
        }

        loadPlayer(&player);
        loadHotbar();
        loadWorld(svc);

        closeSave();
    }
    else
    {
        setMenuFlag(MENU_FLAG_NOSAVE);
    }
}

void calcFrameMenu()
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
            case MENU_SELECTION_MANAGE_GAMES:
            {
                state = STATE_MANAGE_GAMES;
                break;
            }
            case MENU_SELECTION_OPTIONS:
            {
                state = STATE_OPTIONS;
                break;
            }
            case MENU_SELECTION_CREDITS:
            {
                state = STATE_CREDITS;
                break;
            }
            case MENU_SELECTION_QUIT:
            {
                running = false;
                break;
            }
        }
    }
}

void calcFrame(uint32_t ticks)
{
    switch(state)
    {
        case STATE_GAME:
        {
            calcFrameGame(&player, &state, ticks, invertY);
            break;
        }
        case STATE_INVENTORY:
        {
            calcFrameInventory(&state);
            break;
        }
        case STATE_MENU:
        {
            calcFrameMenu();
            break;
        }
        case STATE_OPTIONS:
        {
            if(keyUp(B_UP))
            {
                scrollOptions(-1);
            }
            else if(keyUp(B_DOWN))
            {
                scrollOptions(1);
            }

            if(keyUp(B_A))
            {
                switch(getOptionsCursor())
                {
                    case OPTION_SELECTION_INVERTY:
                    {
                        invertY = !invertY;
                        break;
                    }
                    case OPTION_SELECTION_SEED:
                    {
                        newGameSeed++;
                        break;
                    }
                    case OPTION_SELECTION_BACK:
                    {
                        saveOptions();
                        state = STATE_MENU;
                        break;
                    }
                }
            }
            else if(keyUp(B_B) && getOptionsCursor() == OPTION_SELECTION_SEED)
            {
                newGameSeed--;
            }
            break;
        }
        case STATE_CREDITS:
        {
            if(keyUp(B_A) || keyUp(B_B) || keyUp(B_START))
            {
                state = STATE_MENU;
            }
            break;
        }
        case STATE_PROGRAMMING:
        {
            calcFrameProgramming(&player, &state, ticks);
            break;
        }
        case STATE_MANAGE_GAMES:
        {
            if(keyUp(B_UP))
            {
                scrollManageGames(-1);
            }
            else if(keyUp(B_DOWN))
            {
                scrollManageGames(1);
            }

            if(keyUp(B_A))
            {
                switch(getManageGamesCursor())
                {
                    case MG_SELECTION_BACK:
                    {
                        state = STATE_MENU;
                        break;
                    }
                    default:
                    {
                        state = STATE_MANAGE_SELECTED_GAME;
                        break;
                    }
                }
            }
            break;
        }
        case STATE_MANAGE_SELECTED_GAME:
        {
            if(keyUp(B_UP))
            {
                scrollManageSelectedGame(-1);
            }
            else if(keyUp(B_DOWN))
            {
                scrollManageSelectedGame(1);
            }

            if(keyUp(B_A))
            {
                switch(getManageSelectedGameCursor())
                {
                    case MSG_SELECTION_LOAD_GAME:
                    {
                        gameIndex = getManageGamesCursor();

                        char saveName[SAVE_NAME_LENGTH + 1];
                        getSaveNameForIndex(saveName, gameIndex);
                        loadGame(saveName);
                        //TODO: we may have to do a little more work here?
                        precalcGame(&player, &state, 1, invertY);

                        state = STATE_GAME;
                        break;
                    }
                    case MSG_SELECTION_NEW_GAME:
                    {
                        gameIndex = getManageGamesCursor();

                        //Destroy old game, initialize new one
                        quitWorld();
                        initWorld(newGameSeed);
                        player.position = (vec3) {0, 0, 0};
                        player.rotation = (vec3) {0, 0, 0};
                        resetHotbar();
                        precalcGame(&player, &state, 1, invertY);

                        state = STATE_GAME;
                        break;
                    }
                    case MSG_SELECTION_BACK:
                    {
                        state = STATE_MANAGE_GAMES;
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
    switch(state)
    {
        case STATE_GAME:
        {
            //Crosshair
            drawTexQuad(WINX / 2 - 8, WINY / 2 - 8, 16, 16, 20, PTC(240), PTC(64), PTC(240 + 15), PTC(64 + 15));
            drawHotbar();
            break;
        }
        case STATE_INVENTORY:
        {
            drawInventory();
            break;
        }
        case STATE_PROGRAMMING:
        {
            drawProgrammingScreen(programmingComputer);
            break;
        }
        case STATE_MENU:
        {
            drawMenu();
            break;
        }
        case STATE_OPTIONS:
        {
            drawOptions(invertY, newGameSeed);
            break;
        }
        case STATE_CREDITS:
        {
            drawCredits();
            break;
        }
        case STATE_MANAGE_GAMES:
        {
            drawManageGamesMenu();
            break;
        }
        case STATE_MANAGE_SELECTED_GAME:
        {
            drawManageSelectedGameMenu();
            break;
        }
    }
    glEnd();

    #ifdef DEBUG
    drawFPS(fps);
    #endif

    flipFrame();
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
    initVideo((vec4) {.d = {0.7f, 0.95f, 1.0f, 1.0f}}, (vec4) {.d = {0, 0, WINX, WINY}}, 70, 0.3f, 8 * VIEW_DISTANCE);
    
    initAudio(MIX_MAX_VOLUME, 1, NUM_SFX);
    loadMusic(0, RESOURCE("res/mus/curiouscritters.ogg"));
    loadSFX();
    playMusic(0, 0);

    loadOptions();

    player.position = (vec3) {0, 0, 0};

    initWorld(newGameSeed);

    //See what saves exist
    checkGamesPresent();
    //Grab save index
    gameIndex = loadGameIndex();
    //TODO: set manage games cursor here?

    if(argc > 1 && strcmp(argv[1], "-skipmenu") == 0)
    {
        state = STATE_GAME;
        loadGame(INSTANTPLAY_SAVE_NAME);
    }
    else
    {
        char saveName[SAVE_NAME_LENGTH + 1];
        getSaveNameForIndex(saveName, gameIndex);
        loadGame(saveName);
    }

    precalcGame(&player, &state, 1, invertY);

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

        #ifndef NO_FPS_LIMIT
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
        char saveName[SAVE_NAME_LENGTH + 1];
        getSaveNameForIndex(saveName, gameIndex);
        saveGame(saveName);
    }
    //Save the game index so we know which save to load
    //In case of instant play, we also need the game index to know which game the instant play save belongs to
    saveGameIndex(gameIndex);

    //Cleanup
    quitWorld();
    
    quitAudio();
	quitVideo();

	return 0;
}