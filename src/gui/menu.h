#ifndef _MENU_H
#define _MENU_H

#include <stdint.h>
#include <stdbool.h>

void drawMenu();

#define MENU_SIZE 4
#define MENU_SELECTION_CONTINUE 0
#define MENU_SELECTION_NEW_GAME 1
#define MENU_SELECTION_OPTIONS  2
#define MENU_SELECTION_QUIT     3

void scrollMenu(int8_t dir);
int8_t getMenuCursor();

#define MENU_FLAG_NOSAVE   0b00000001
#define MENU_FLAG_LOADFAIL 0b00000010

void setMenuFlag(uint8_t flag);

void drawOptions(bool invertY, uint32_t seed);

#define OPTIONS_SIZE 3
#define OPTION_SELECTION_INVERTY 0
#define OPTION_SELECTION_SEED    1
#define OPTION_SELECTION_BACK    2

void scrollOptions(int8_t dir);
int8_t getOptionsCursor();

#endif