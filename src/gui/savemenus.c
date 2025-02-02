#include "savemenus.h"

#include "../fk-engine-core/video.h"
#include "../fk-engine-core/text.h"
#include "../fk-engine-core/input.h"

#include "menu.h"

int8_t manageGamesCursor = 0;
int8_t manageSelectedGameCursor = 0;

void drawManageGamesMenu()
{
    drawTitle("Manage games");

    //Draw save list
    for(uint8_t i = 0; i < NUM_SAVES; i++)
    {
        char saveName[15 + 1];
        char saveNameLength;
        if(gamesPresent[i])
        {
            sprintf(saveName, "Save %02d", i + 1);
            saveNameLength = 7;
        }
        else
        {
            sprintf(saveName, "Save %02d (Empty)", i + 1);
            saveNameLength = 15;
        }

        glDrawText(saveName, CENTER_TEXT(saveNameLength), 96 + i * 16, manageGamesCursor == i ? TEXT_YELLOW : TEXT_WHITE);
    }

    glDrawText("Back", CENTER_TEXT(4), 96 + NUM_SAVES * 16, manageGamesCursor == MG_SELECTION_BACK ? TEXT_YELLOW : TEXT_WHITE);
}

void calcFrameManageGamesMenu(State* state)
{
    if(keyUp(B_UP))
    {
        scrollCursor(&manageGamesCursor, -1, 0, MG_SELECTION_BACK);
    }
    else if(keyUp(B_DOWN))
    {
        scrollCursor(&manageGamesCursor, 1, 0, MG_SELECTION_BACK);
    }

    if(keyUp(B_A))
    {
        switch(manageGamesCursor)
        {
            case MG_SELECTION_BACK:
            {
                *state = STATE_MENU;
                break;
            }
            default:
            {
                if(!gamesPresent[manageGamesCursor])
                {
                    manageSelectedGameCursor = MSG_SELECTION_NEW_GAME;
                }
                *state = STATE_MANAGE_SELECTED_GAME;
                break;
            }
        }
    }
}

const char* manageSelectedGameStrings[MSG_SIZE] = {
    [MSG_SELECTION_LOAD_GAME] = "Load game",
    [MSG_SELECTION_NEW_GAME] = "New game",
    [MSG_SELECTION_BACK] = "Back"
};

void drawManageSelectedGameMenu()
{
    char subtitle[14 + 1];
    sprintf(subtitle, "Manage Save %02d", manageGamesCursor + 1);
    drawTitle(subtitle);

    //Skip "load game" if there's no save to be loaded
    uint8_t startIndex = MSG_SELECTION_LOAD_GAME;
    if(!gamesPresent[manageGamesCursor])
    {
        startIndex = MSG_SELECTION_NEW_GAME;
    }

    for(uint8_t i = startIndex; i < MSG_SIZE; i++)
    {
        glDrawText(manageSelectedGameStrings[i], CENTER_TEXT(strlen(manageSelectedGameStrings[i])), 96 + i * 16, manageSelectedGameCursor == i ? TEXT_YELLOW : TEXT_WHITE);
    }
}

void scrollManageSelectedGame(int8_t dir)
{
    //Skip "load game" if there's no save to be loaded
    uint8_t startIndex = MSG_SELECTION_LOAD_GAME;
    if(!gamesPresent[manageGamesCursor])
    {
        startIndex = MSG_SELECTION_NEW_GAME;
    }

    scrollCursor(&manageSelectedGameCursor, dir, startIndex, MSG_SELECTION_BACK);
}

int8_t getManageSelectedGameCursor()
{
    return manageSelectedGameCursor;
}