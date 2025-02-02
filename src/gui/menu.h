#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdbool.h>

#include "../gameloop.h"

void scrollCursor(int8_t* cursor, int8_t dir, int8_t min, int8_t max);
void drawTitle(const char* subtitle);

typedef enum {
    MENU_SELECTION_CONTINUE = 0,
    MENU_SELECTION_MANAGE_GAMES,
    MENU_SELECTION_OPTIONS,
    MENU_SELECTION_CREDITS,
    MENU_SELECTION_QUIT,
    MENU_SIZE
} MenuSelection;

void drawMenu();

#define MENU_FLAG_NOSAVE   0b00000001
#define MENU_FLAG_LOADFAIL 0b00000010

void setMenuFlag(uint8_t flag);

void calcFrameMenu(State* state, bool* running, Player* player, uint32_t newGameSeed, bool invertY);

#define OPTIONS_SIZE 3
#define OPTION_SELECTION_INVERTY 0
#define OPTION_SELECTION_SEED    1
#define OPTION_SELECTION_BACK    2

void drawOptions(bool invertY, uint32_t seed);
void scrollOptions(int8_t dir);
int8_t getOptionsCursor();

void drawCredits();

#endif