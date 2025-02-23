#include "menu.h"

#include <GL/gl.h>
#include <stdio.h>

#include "../fk-engine-core/video.h"
#include "../fk-engine-core/audio.h"
#include "../fk-engine-core/text.h"
#include "../fk-engine-core/input.h"

#include "../sfx.h"
#include "../version.h"

#include "popup.h"

int8_t menuCursor = 0;
int8_t optionsCursor = 0;

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

const char* menuStringNewGame = "New game";
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
        glDrawText(subtitle, CENTER_TEXT(strlen(subtitle)), 60, TEXT_WHITE);
    }
}

void drawMenu()
{
    drawTitle(GAME_VERSION);

    drawPopupIfActive();

    //Menu selections
    for(uint8_t i = 0; i < MENU_SIZE; i++)
    {
        if(i == MENU_SELECTION_CONTINUE && (menuFlags & MENU_FLAG_NOSAVE))
        {
            glDrawTextCentered(menuStringNewGame, 96 + i * 16, menuCursor == i ? TEXT_YELLOW : TEXT_WHITE);
        }
        else
        {
            glDrawTextCentered(menuStrings[i], 96 + i * 16, menuCursor == i ? TEXT_YELLOW : TEXT_WHITE);
        }
    }

    //Author notice
    glDrawTextCentered("2022 - 2025", 240 - 32, TEXT_WHITE);
    glDrawTextCentered("robbi-blechdose", 240 - 20, TEXT_WHITE);
}

void setMenuFlag(uint8_t flag)
{
    menuFlags |= flag;
}

void calcFrameMenu(State* state, bool* running, Player* player, uint32_t newGameSeed, bool invertY)
{
    if(isPopupOpen())
    {
        if(keyUp(B_START) || keyUp(B_A) || keyUp(B_B))
        {
            closePopup();
        }
        return;
    }

    if(keyUp(B_UP))
    {
        scrollCursor(&menuCursor, -1, 0, MENU_SIZE - 1);
    }
    else if(keyUp(B_DOWN))
    {
        scrollCursor(&menuCursor, 1, 0, MENU_SIZE - 1);
    }
    
    if(keyUp(B_START) || keyUp(B_A))
    {
        switch(menuCursor)
        {
            case MENU_SELECTION_CONTINUE:
            {
                if(menuFlags & MENU_FLAG_NOSAVE)
                {
                    newGame(player, newGameSeed);
                }
                *state = STATE_GAME;
                break;
            }
            case MENU_SELECTION_MANAGE_GAMES:
            {
                *state = STATE_MANAGE_GAMES;
                break;
            }
            case MENU_SELECTION_OPTIONS:
            {
                *state = STATE_OPTIONS;
                break;
            }
            case MENU_SELECTION_CREDITS:
            {
                *state = STATE_CREDITS;
                break;
            }
            case MENU_SELECTION_QUIT:
            {
                *running = false;
                break;
            }
        }
    }
}

void drawOptions(bool invertY, uint32_t seed)
{
    drawTitle("Options");

    char buffer[31];
    sprintf(buffer, "Invert Y: %s", invertY ? "On" : "Off");
    glDrawTextCentered(buffer, 96, optionsCursor == 0 ? TEXT_YELLOW : TEXT_WHITE);

    sprintf(buffer, "Seed: %d", seed);
    glDrawTextCentered(buffer, 96 + 16, optionsCursor == 1 ? TEXT_YELLOW : TEXT_WHITE);

    glDrawTextCentered("Back", 96 + 16 * 2, optionsCursor == 2 ? TEXT_YELLOW : TEXT_WHITE);
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
    "robbi-blechdose",
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
        glDrawTextCentered(creditsLines[i], 64 + i * 16, TEXT_WHITE);
    }
}