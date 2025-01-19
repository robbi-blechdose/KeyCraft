#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdbool.h>

#include "../saves.h"

typedef enum {
    MENU_SELECTION_CONTINUE = 0,
    MENU_SELECTION_MANAGE_GAMES,
    MENU_SELECTION_OPTIONS,
    MENU_SELECTION_CREDITS,
    MENU_SELECTION_QUIT,
    MENU_SIZE
} MenuSelection;

void drawMenu();
void scrollMenu(int8_t dir);
int8_t getMenuCursor();

#define MENU_FLAG_NOSAVE   0b00000001
#define MENU_FLAG_LOADFAIL 0b00000010

void setMenuFlag(uint8_t flag);

typedef enum {
    MG_SELECTION_LOAD_GAME0 = 0,
    MG_SELECTION_BACK = (MG_SELECTION_LOAD_GAME0 + NUM_SAVES)
} ManageGamesSelection;

void drawManageGamesMenu();
void scrollManageGames(int8_t dir);
int8_t getManageGamesCursor();

//TODO: maybe move the seed here too?

typedef enum {
    MSG_SELECTION_LOAD_GAME = 0,
    MSG_SELECTION_NEW_GAME,
    MSG_SELECTION_BACK,
    MSG_SIZE
} ManageSelectedGameSelection;

void drawManageSelectedGameMenu();
void scrollManageSelectedGame(int8_t dir);
int8_t getManageSelectedGameCursor();

#define OPTIONS_SIZE 3
#define OPTION_SELECTION_INVERTY 0
#define OPTION_SELECTION_SEED    1
#define OPTION_SELECTION_BACK    2

void drawOptions(bool invertY, uint32_t seed);
void scrollOptions(int8_t dir);
int8_t getOptionsCursor();

void drawCredits();

#endif