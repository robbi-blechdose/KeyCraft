#include "saves.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "engine/savegame.h"

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
        getSaveNameForIndex(&saveName, i);
        gamesPresent[i] = doesGameExist(saveName);
    }
}