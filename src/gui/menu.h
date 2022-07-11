#ifndef _MENU_H
#define _MENU_H

#include <stdint.h>

void drawMenu();

void scrollMenu(int8_t dir);

#define MENU_SELECTION_CONTINUE 0
#define MENU_SELECTION_NEW_GAME 1
#define MENU_SELECTION_OPTIONS  2
#define MENU_SELECTION_QUIT     3
uint8_t getMenuCursor();

#define MENU_FLAG_NOSAVE   0b00000001
#define MENU_FLAG_LOADFAIL 0b00000010

void setMenuFlag(uint8_t flag);

#endif