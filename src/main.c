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
#include "gui/programming.h"

#include "saves.h"

//TODO: instant play needs to remember which save index it is!

#define MAX_FPS 50

#ifdef DEBUG
uint16_t fps;
#define FPS_WINDOW 250
uint32_t counterFrames = 0;
uint32_t counterTime = 0;
#endif

typedef enum {
    STATE_GAME,
    STATE_INVENTORY,
    STATE_MENU,
    STATE_PROGRAMMING,
    STATE_OPTIONS,
    STATE_CREDITS,
    STATE_MANAGE_GAMES,
    STATE_MANAGE_SELECTED_GAME
} State;

#define SHELL_CMD_POWERDOWN_HANDLE "powerdown handle"
#define SHELL_CMD_INSTANT_PLAY     "instant_play"

char* programName = NULL;

//---------- Main game stuff ----------//
bool running = true;
bool quickSaveAndPoweroff = false;
State state = STATE_MENU;
Player player;
ComputerData* programmingComputer;

//Options
bool invertY = true;
//TODO: each save will have to save its own seed
uint32_t newGameSeed = 0;
//TODO
uint8_t gameIndex = 0;

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

inline void tryPlaceBlockOrInteract(BlockPos* block, AABBSide result)
{
    if(actWorldBlock(block))
    {
        return;
    }
    
    if(getWorldBlock(block)->type == BLOCK_COMPUTER)
    {
        programmingComputer = getWorldChunk(block)->computers[GET_COMPUTER_INDEX(getWorldBlock(block)->data)];
        state = STATE_PROGRAMMING;
        return;
    }

    //Calc position
    switch(result)
    {
        case AABB_FRONT:
        {
            block->z -= BLOCK_SIZE;
            break;
        }
        case AABB_BACK:
        {
            block->z += BLOCK_SIZE;
            break;
        }
        case AABB_LEFT:
        {
            block->x -= BLOCK_SIZE;
            break;
        }
        case AABB_RIGHT:
        {
            block->x += BLOCK_SIZE;
            break;
        }
        case AABB_BOTTOM:
        {
            block->y -= BLOCK_SIZE;
            break;
        }
        case AABB_TOP:
        {
            block->y += BLOCK_SIZE;
            break;
        }
        default:
        {
            break;
        }
    }

    if(getWorldBlock(block) == NULL || getWorldBlock(block)->type != BLOCK_AIR)
    {
        return;
    }

    //Place new block
    BlockPos below = *block;
    below.y -= BLOCK_SIZE;
    BlockPos above = *block;
    above.y += BLOCK_SIZE;
    Block toPlace = getHotbarSelection();

    if(!canPlaceBlock(toPlace.type, getWorldBlock(&below)->type) ||
        (toPlace.type == BLOCK_DOOR && (getWorldBlock(&above) == NULL || getWorldBlock(&above)->type != BLOCK_AIR))) //Check if we can place the door upper (yes, this is a special case)
    {
        return;
    }

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

    setWorldBlock(block, toPlace);
    //Check if the block intersects with the player. If so, don't place it
    if(playerIntersectsWorld(&player))
    {
        //Remove door upper as well (yes, this is a special case)
        if(toPlace.type == BLOCK_DOOR)
        {
            setWorldBlock(&above, (Block) {BLOCK_AIR, 0});
        }
        else if(toPlace.type == BLOCK_COMPUTER)
        {
            //Remove computer data
            free(getWorldChunk(block)->computers[GET_COMPUTER_INDEX(getWorldBlock(block)->data)]);
            getWorldChunk(block)->computers[GET_COMPUTER_INDEX(getWorldBlock(block)->data)] = NULL;
        }
        setWorldBlock(block, (Block) {BLOCK_AIR, 0});
    }
}

inline void tryRemoveBlock(BlockPos* block)
{
    Block* toRemove = getWorldBlock(block);
    
    if(toRemove->type == BLOCK_BEDROCK)
    {
        return;
    }
    
    //Remove other door half (yes, this is a special case)
    if(toRemove->type == BLOCK_DOOR)
    {
        if(toRemove->data & BLOCK_DATA_PART)
        {
            //This is the upper part, remove lower
            block->y--;
            setWorldBlock(block, (Block) {BLOCK_AIR, 0});
            block->y++;
        }
        else
        {
            //This is the lower part, remove upper
            block->y++;
            setWorldBlock(block, (Block) {BLOCK_AIR, 0});
            block->y--;
        }
    }
    //Remove other half of piston (yes, this is a special case)
    else if(toRemove->type == BLOCK_PISTON_BASE)
    {
        BlockPos pos = {block->chunk, block->x, block->y, block->z};
        getBlockPosByDirection(toRemove->data & BLOCK_DATA_DIRECTION, &pos);
        setWorldBlock(&pos, (Block) {BLOCK_AIR, 0});
    }
    else if(toRemove->type == BLOCK_PISTON_HEAD)
    {
        BlockPos pos = {block->chunk, block->x, block->y, block->z};
        getBlockPosByInverseDirection(toRemove->data & BLOCK_DATA_DIRECTION, &pos);
        setWorldBlock(&pos, (Block) {BLOCK_AIR, 0});
    }
    else if(toRemove->type == BLOCK_COMPUTER)
    {
        //Remove computer data
        free(getWorldChunk(block)->computers[GET_COMPUTER_INDEX(toRemove->data)]);
        getWorldChunk(block)->computers[GET_COMPUTER_INDEX(toRemove->data)] = NULL;
    }

    setWorldBlock(block, (Block) {BLOCK_AIR, 0});
}

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
    if(!invertY)
    {
        dirX *= -1;
    }
    playerLook(&player, dirX, dirY, ticks);

    if(keyDown(B_Y) && !player.jumping)
    {
        player.jumping = JUMP_TIME;
    }

    calcPlayer(&player, ticks);

    //Cast ray
    vec3 rayDir = anglesToDirection(&player.rotation);
    //Player position in world space
    vec3 posWorld = addv3(player.position, (vec3) {VIEW_TRANSLATION, VIEW_TRANSLATION, VIEW_TRANSLATION});

    BlockPos block;
    float distance;
    AABBSide result = intersectsRayWorld(&posWorld, &rayDir, &block, &distance);

    if(keyUp(B_A) && result)
    {
        tryPlaceBlockOrInteract(&block, result);
    }
    //Remove block
    else if(keyUp(B_B) && result)
    {
        tryRemoveBlock(&block);
    }

    if(keyUp(B_SELECT))
    {
        scrollHotbar();
    }
    else if(keyUp(B_START))
    {
        state = STATE_INVENTORY;
    }
    else if(keyUp(B_MENU))
    {
        state = STATE_MENU;
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
            if(keyUp(B_TL) || keyUp(B_X))
            {
                dirTab = -1;
            }
            else if(keyUp(B_TR) || keyUp(B_Y))
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
            else if(keyUp(B_START))
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
            calcWorld(&player.position, ticks);

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
            moveProgrammingCursor(dirX, dirY);

            if(keyUp(B_A))
            {
                enterProgrammingCursor(programmingComputer);
            }
            else if(keyUp(B_B))
            {
                if(!cancelProgrammingCursor())
                {
                    state = STATE_GAME;
                }
            }
            else if(keyUp(B_X))
            {
                shiftProgramDown(programmingComputer);
            }
            else if(keyUp(B_Y))
            {
                shiftProgramUp(programmingComputer);
            }
            else if(keyUp(B_START))
            {
                if(programmingComputer->af & COMPUTER_FLAG_RUNNING)
                {
                    programmingComputer->af &= ~COMPUTER_FLAG_RUNNING;
                }
                else
                {
                    programmingComputer->af |= COMPUTER_FLAG_RUNNING;
                }
            }

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
                        char saveName[SAVE_NAME_LENGTH + 1];
                        getSaveNameForIndex(saveName, getManageGamesCursor());
                        loadGame(saveName);
                        //TODO: we may have to do a little more work here
                        break;
                    }
                    case MSG_SELECTION_NEW_GAME:
                    {
                        state = STATE_GAME;

                        //Destroy old game, initialize new one
                        quitWorld();
                        initWorld(newGameSeed);
                        player.position = (vec3) {0, 0, 0};
                        player.rotation = (vec3) {0, 0, 0};
                        //TODO: reinit hotbar
                        //Run frame to build geometry for the first time etc.
                        calcFrameGame(1);
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

//TODO: fix segfault on saving with no modified chunks!

void saveGame(char* name)
{
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
    initVideo((vec4) {.d = {0.7f, 0.95f, 1.0f, 1.0f}}, (vec4) {.d = {0, 0, WINX, WINY}}, 70, 0.3f, 8 * VIEW_DISTANCE);
    
    initAudio(MIX_MAX_VOLUME, 1, NUM_SFX);
    loadMusic(0, RESOURCE("res/mus/curiouscritters.ogg"));
    loadSampleIndex(SFX_LEVER, RESOURCE("res/sfx/click20.ogg"));
    loadSampleIndex(SFX_TNT, RESOURCE("res/sfx/explosion.ogg"));
    loadSampleIndex(SFX_DOOR, RESOURCE("res/sfx/wooded_box_open.ogg"));
    loadSampleIndex(SFX_MENU, RESOURCE("res/sfx/click1.ogg"));
    loadSampleIndex(SFX_PRESSURE_PLATE, RESOURCE("res/sfx/click27.ogg"));
    loadSampleIndex(SFX_PRESSURE_PLATE_OFF, RESOURCE("res/sfx/click28.ogg"));
    //Noteblock notes
    loadSampleIndex(SFX_NOTEBLOCK_0, RESOURCE("res/sfx/noteblock/key05.ogg"));
    loadSampleIndex(SFX_NOTEBLOCK_0 + 1, RESOURCE("res/sfx/noteblock/key06.ogg"));
    loadSampleIndex(SFX_NOTEBLOCK_0 + 2, RESOURCE("res/sfx/noteblock/key07.ogg"));
    loadSampleIndex(SFX_NOTEBLOCK_0 + 3, RESOURCE("res/sfx/noteblock/key08.ogg"));
    loadSampleIndex(SFX_NOTEBLOCK_0 + 4, RESOURCE("res/sfx/noteblock/key09.ogg"));
    loadSampleIndex(SFX_NOTEBLOCK_0 + 5, RESOURCE("res/sfx/noteblock/key10.ogg"));
    loadSampleIndex(SFX_NOTEBLOCK_0 + 6, RESOURCE("res/sfx/noteblock/key11.ogg"));
    loadSampleIndex(SFX_NOTEBLOCK_0 + 7, RESOURCE("res/sfx/noteblock/key12.ogg"));
    //Noteblock notes end
    playMusic(0, 0);

    loadOptions();

    player.position = (vec3) {0, 0, 0};

    initWorld(newGameSeed);

    //See what saves exist
    checkGamesPresent();

    if(argc > 1 && strcmp(argv[1], "-skipmenu") == 0)
    {
        state = STATE_GAME;
        loadGame(INSTANTPLAY_SAVE_NAME);
    }
    else
    {
        loadGame(SAVE_BASENAME SAVE_EXTENSION);
    }

    //Run frames to build geometry for the first time etc.
    for(uint8_t i = 0; i < (VIEW_DISTANCE * VIEW_DISTANCE * VIEW_DISTANCE) / MAX_CHUNKS_PER_FRAME; i++)
    {
        calcFrameGame(1);
    }

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
        saveGame(SAVE_BASENAME SAVE_EXTENSION);
    }

    //Cleanup
    quitWorld();
    
    quitAudio();
	quitVideo();

	return 0;
}