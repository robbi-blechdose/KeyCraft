#include "menu.h"

#include <GL/gl.h>
#include "../engine/video.h"

/**
 * Macro for centering text
 * X is the length of the text
 * Note: X * 8 / 2 = X * 4
 **/
#define CENTER(X) (WINX / 2 - (X) * 4)

#define MENU_SIZE 3
int8_t menuCursor = 0;

const char* menuStrings[] = {
    "Continue",
    "New game",
    "Options"
};

void drawMenu()
{
    //Title
    glTextSize(GL_TEXT_SIZE16x16);
    glDrawText("KeyCraft", WINX / 2 - (8 * 16 / 2), 64, TEXT_WHITE);
    
    //Menu selections
    glTextSize(GL_TEXT_SIZE8x8);
    for(uint8_t i = 0; i < MENU_SIZE; i++)
    {
        glDrawText(menuStrings[i], CENTER(strlen(menuStrings[i])), 96 + i * 16, menuCursor == i ? TEXT_YELLOW : TEXT_WHITE);
    }

    //Author notice
    glDrawText("2022", CENTER(4), 240 - 32, TEXT_WHITE);
    glDrawText("Robbi Blechdose", CENTER(15), 240 - 20, TEXT_WHITE);
}

void scrollMenu(int8_t dir)
{
    menuCursor += dir;
    if(menuCursor < 0)
    {
        menuCursor = MENU_SIZE - 1;
    }
    else if(menuCursor >= MENU_SIZE)
    {
        menuCursor = 0;
    }
}

uint8_t getMenuCursor()
{
    return menuCursor;
}