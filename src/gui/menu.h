#ifndef _MENU_H
#define _MENU_H

#include <stdint.h>

void drawMenu();

void scrollMenu(int8_t dir);

#define MENU_SELECTION_CONTINUE 0
#define MENU_SELECTION_NEW_GAME 1
#define MENU_SELECTION_OPTIONS  2
uint8_t getMenuCursor();

#endif