#include "saves.h"

#include <stdio.h>
#include <string.h>
#include "fk-engine-core/savegame.h"

#include "inventory.h"
#include "world.h"

SaveVersionCompat readSaveVersionCompat()
{
    uint16_t saveVersion;
    readElement(&saveVersion, sizeof(uint16_t));

    if(saveVersion == SAVE_VERSION)
    {
        return SV_COMPAT_OK;
    }
    else if(saveVersion / 10 == SAVE_VERSION / 10)
    {
        return SV_COMPAT_MINOR;
    }
    else if(saveVersion / 10 == (SAVE_VERSION / 10) - 1)
    {
        return SV_COMPAT_MAJOR;
    }
    else
    {
        return SV_COMPAT_NONE;
    }
}

bool gamesPresent[NUM_SAVES];

bool doesGameExist(char* name)
{
    if(openSave(SAVE_FOLDER, name, false))
    {
        closeSave();
        return true;
    }
    return false;
}

void getSaveNameForIndex(char* buffer, uint8_t index)
{
    sprintf(buffer, SAVE_BASENAME "%02d" SAVE_EXTENSION, index);
}

void checkGamesPresent()
{
    for(uint8_t i = 0; i < NUM_SAVES; i++)
    {
        char saveName[SAVE_NAME_LENGTH + 1];
        getSaveNameForIndex(saveName, i);
        gamesPresent[i] = doesGameExist(saveName);
    }
}

void saveGameIndex(uint8_t gameIndex)
{
    if(openSave(SAVE_FOLDER, GAME_INDEX_NAME, true))
    {
        writeElement(&gameIndex, sizeof(uint8_t));
        closeSave();
    }
}

uint8_t loadGameIndex()
{
    if(openSave(SAVE_FOLDER, GAME_INDEX_NAME, false))
    {
        uint8_t gameIndex;
        readElement(&gameIndex, sizeof(uint8_t));
        closeSave();
        return gameIndex;
    }
    else
    {
        return GAME_INDEX_NONE;
    }
}

SaveResult saveGame(char* name, Player* player)
{
    //Skip saving if the world is unmodified
    //That means we lose the hotbar and player position, but that's a sacrifice I'm willing to make
    if(isWorldUnmodified())
    {
        return SR_NOCHANGE;
    }

    if(openSave(SAVE_FOLDER, name, true))
    {
        uint16_t saveVersion = SAVE_VERSION;
        writeElement(&saveVersion, sizeof(uint16_t));

        savePlayer(player);
        saveHotbar();
        saveWorld();
        closeSave();
        return SR_OK;
    }

    return SR_FAIL_OPEN;
}

LoadResult loadGame(char* name, Player* player)
{
    if(openSave(SAVE_FOLDER, name, false))
    {
        SaveVersionCompat svc = readSaveVersionCompat();
        if(svc == SV_COMPAT_NONE)
        {
            closeSave();
            return LR_FAIL_SVC;
        }

        loadPlayer(player);
        loadHotbar();
        loadWorld(svc);

        closeSave();
        return LR_OK;
    }

    return LR_FAIL_OPEN;
}