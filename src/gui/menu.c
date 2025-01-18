#include "menu.h"

#include <GL/gl.h>
#include <stdio.h>

#include "../engine/video.h"
#include "../engine/audio.h"

#include "../sfx.h"
#include "../version.h"

/**
 * Macro for centering text
 * X is the length of the text
 * Note: X * 8 / 2 = X * 4
 **/
#define CENTER(X) (WINX / 2 - (X) * 4)

int8_t menuCursor = 0;
int8_t optionsCursor = 0;

uint8_t menuFlags = 0;

const char* menuStrings[MENU_SIZE] = {
    "Continue",
    "New game",
    "Options",
    "Credits",
    "Quit"
};

void drawTitle()
{
    glTextSize(GL_TEXT_SIZE16x16);
    glDrawText("KeyCraft", WINX / 2 - (8 * 16 / 2) + 2, 40 + 2, TEXT_BLACK);
    glDrawText("KeyCraft", WINX / 2 - (8 * 16 / 2), 40, TEXT_WHITE);
    glTextSize(GL_TEXT_SIZE8x8);
}

void drawMenu()
{
    drawTitle();

    //Game version
    glDrawText(GAME_VERSION, WINX / 2 - strlen(GAME_VERSION) * 8 / 2, 60, TEXT_WHITE);
    
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
    menuCursor += dir;

    uint8_t min = 0;

    if(menuFlags & (MENU_FLAG_NOSAVE | MENU_FLAG_LOADFAIL))
    {
        min++;
    }

    if(menuCursor < min)
    {
        menuCursor = MENU_SIZE - 1;
    }
    else if(menuCursor >= MENU_SIZE)
    {
        menuCursor = min;
    }

    playSample(SFX_MENU);
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

void drawOptions(bool invertY, uint32_t seed)
{
    glDrawText("KeyCraft Options", CENTER(16), 64, TEXT_WHITE);

    char buffer[31];
    sprintf(buffer, "Invert Y: %s", invertY ? "On" : "Off");
    glDrawText(buffer, CENTER(strlen(buffer)), 96, optionsCursor == 0 ? TEXT_YELLOW : TEXT_WHITE);

    sprintf(buffer, "Seed: %d", seed);
    glDrawText(buffer, CENTER(strlen(buffer)), 96 + 16, optionsCursor == 1 ? TEXT_YELLOW : TEXT_WHITE);

    glDrawText("Back", CENTER(4), 96 + 16 * 2, optionsCursor == 2 ? TEXT_YELLOW : TEXT_WHITE);
}

void scrollOptions(int8_t dir)
{
    optionsCursor += dir;

    if(optionsCursor < 0)
    {
        optionsCursor = OPTIONS_SIZE - 1;
    }
    else if(optionsCursor >= OPTIONS_SIZE)
    {
        optionsCursor = 0;
    }

    playSample(SFX_MENU);
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
    drawTitle();

    for(uint8_t i = 0; i < CREDITS_LINES; i++)
    {
        glDrawText(creditsLines[i], CENTER(strlen(creditsLines[i])), 64 + i * 16, TEXT_WHITE);
    }
}