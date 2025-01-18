#include "menu.h"

#include <GL/gl.h>
#include <stdio.h>

#include "../engine/video.h"
#include "../engine/audio.h"

#include "../sfx.h"
#include "../version.h"

//TODO: unify and move somewhere else

/**
 * Macro for centering text
 * X is the length of the text
 * Note: X * 8 / 2 = X * 4
 **/
#define CENTER(X) (WINX / 2 - (X) * 4)

//Fixes warnings for string literals
#define glDrawText(text, x, y, pixel) glDrawText((const GLubyte*) (text), (x), (y), (pixel))

int8_t menuCursor = 0;
int8_t optionsCursor = 0;
int8_t manageGamesCursor = 0;
int8_t manageSelectedGameCursor = 0;

uint8_t menuFlags = 0;

void scrollCursor(int8_t* cursor, int8_t dir, int8_t min, int8_t max)
{
    *cursor += dir;

    if(*cursor < min)
    {
        *cursor = max;
    }
    else if(*cursor > max)
    {
        *cursor = min;
    }

    playSample(SFX_MENU);
}

const char* menuStrings[MENU_SIZE] = {
    [MENU_SELECTION_CONTINUE] = "Continue",
    [MENU_SELECTION_MANAGE_GAMES] = "Manage games",
    [MENU_SELECTION_OPTIONS] = "Options",
    [MENU_SELECTION_CREDITS] = "Credits",
    [MENU_SELECTION_QUIT] = "Quit"
};

void drawTitle(const char* subtitle)
{
    glTextSize(GL_TEXT_SIZE16x16);
    glDrawText("KeyCraft", WINX / 2 - (8 * 16 / 2) + 2, 40 + 2, TEXT_BLACK);
    glDrawText("KeyCraft", WINX / 2 - (8 * 16 / 2), 40, TEXT_WHITE);
    glTextSize(GL_TEXT_SIZE8x8);

    if(subtitle != NULL)
    {
        glDrawText(subtitle, CENTER(strlen(subtitle)), 60, TEXT_WHITE);
    }
}

void drawMenu()
{
    drawTitle(GAME_VERSION);

    //TODO: redo handling of "nosave": replace "continue" with "new game" if no save is detected

    //Menu selections
    for(uint8_t i = 0; i < MENU_SIZE; i++)
    {
        if(i == MENU_SELECTION_CONTINUE)
        {
            if(menuFlags & MENU_FLAG_NOSAVE)
            {
                continue;
            }
            else if(menuFlags & MENU_FLAG_LOADFAIL)
            {
                glDrawText("Failed to load save.", CENTER(20), 96 + i * 16, TEXT_WHITE);
                continue;
            }
        }
        glDrawText(menuStrings[i], CENTER(strlen(menuStrings[i])), 96 + i * 16, menuCursor == i ? TEXT_YELLOW : TEXT_WHITE);
    }

    //Author notice
    glDrawText("2022 - 2025", CENTER(11), 240 - 32, TEXT_WHITE);
    glDrawText("Robbi Blechdose", CENTER(15), 240 - 20, TEXT_WHITE);
}

void scrollMenu(int8_t dir)
{
    uint8_t min = 0;
    if(menuFlags & (MENU_FLAG_NOSAVE | MENU_FLAG_LOADFAIL))
    {
        min++;
    }

    scrollCursor(&menuCursor, dir, min, MENU_SIZE - 1);
}

int8_t getMenuCursor()
{
    return menuCursor;
}

void setMenuFlag(uint8_t flag)
{
    menuFlags |= flag;
    if((flag == MENU_FLAG_NOSAVE || flag == MENU_FLAG_LOADFAIL) && menuCursor == 0)
    {
        menuCursor = 1;
    }
}

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

        glDrawText(saveName, CENTER(saveNameLength), 96 + i * 16, manageGamesCursor == i ? TEXT_YELLOW : TEXT_WHITE);
    }

    glDrawText("Back", CENTER(4), 96 + NUM_SAVES * 16, manageGamesCursor == MG_SELECTION_BACK ? TEXT_YELLOW : TEXT_WHITE);
}

void scrollManageGames(int8_t dir)
{
    scrollCursor(&manageGamesCursor, dir, 0, MG_SELECTION_BACK);
}

int8_t getManageGamesCursor()
{
    return manageGamesCursor;
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

    for(uint8_t i = 0; i < MSG_SIZE; i++)
    {
        glDrawText(manageSelectedGameStrings[i], CENTER(strlen(manageSelectedGameStrings[i])), 96 + i * 16, manageSelectedGameCursor == i ? TEXT_YELLOW : TEXT_WHITE);
    }
}

void scrollManageSelectedGame(int8_t dir)
{
    scrollCursor(&manageSelectedGameCursor, dir, 0, MSG_SELECTION_BACK);
}

int8_t getManageSelectedGameCursor()
{
    return manageSelectedGameCursor;
}

void drawOptions(bool invertY, uint32_t seed)
{
    drawTitle("Options");

    char buffer[31];
    sprintf(buffer, "Invert Y: %s", invertY ? "On" : "Off");
    glDrawText(buffer, CENTER(strlen(buffer)), 96, optionsCursor == 0 ? TEXT_YELLOW : TEXT_WHITE);

    sprintf(buffer, "Seed: %d", seed);
    glDrawText(buffer, CENTER(strlen(buffer)), 96 + 16, optionsCursor == 1 ? TEXT_YELLOW : TEXT_WHITE);

    glDrawText("Back", CENTER(4), 96 + 16 * 2, optionsCursor == 2 ? TEXT_YELLOW : TEXT_WHITE);
}

void scrollOptions(int8_t dir)
{
    scrollCursor(&optionsCursor, dir, 0, OPTIONS_SIZE - 1);
}

int8_t getOptionsCursor()
{
    return optionsCursor;
}

#define CREDITS_LINES 10

const char* creditsLines[CREDITS_LINES] = {
    "Programming and Graphics:",
    "Robbi Blechdose",
    "Music:",
    "Curious Critters",
    "  by Matthew Pablo",
    "SFX:",
    "OwlishMedia",
    "Q009",
    "rubberduck",
    "from opengameart.org"
};

void drawCredits()
{
    drawTitle(NULL);

    for(uint8_t i = 0; i < CREDITS_LINES; i++)
    {
        glDrawText(creditsLines[i], CENTER(strlen(creditsLines[i])), 64 + i * 16, TEXT_WHITE);
    }
}