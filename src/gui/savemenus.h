#ifndef SAVE_MENUS_H
#define SAVE_MENUS_H

#include "../saves.h"
#include "../gameloop.h"

typedef enum {
    MG_SELECTION_LOAD_GAME0 = 0,
    MG_SELECTION_BACK = (MG_SELECTION_LOAD_GAME0 + NUM_SAVES)
} ManageGamesSelection;

extern int8_t manageGamesCursor;
void drawManageGamesMenu();
void calcFrameManageGamesMenu(State* state);

typedef enum {
    MSG_SELECTION_LOAD_GAME = 0,
    MSG_SELECTION_NEW_GAME,
    MSG_SELECTION_BACK,
    MSG_SIZE
} ManageSelectedGameSelection;

void drawManageSelectedGameMenu();
void scrollManageSelectedGame(int8_t dir);
int8_t getManageSelectedGameCursor();

#endif