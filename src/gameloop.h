#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "player.h"
#include "computer.h"

typedef enum {
    STATE_GAME,
    STATE_INVENTORY,
    STATE_MENU,
    STATE_PROGRAMMING,
    STATE_OPTIONS,
    STATE_CREDITS,
    STATE_MANAGE_GAMES,
    STATE_MANAGE_SELECTED_GAME
} State;

extern ComputerData* programmingComputer;

void calcFrameGame(Player* player, State* state, uint32_t ticks, bool invertY);

/**
 * Run the game for enough frames to build all geometry.
 * Used for game loading
 */
void precalcGame(Player* player, uint32_t ticks);

void newGame(Player* player, uint32_t newGameSeed);

void calcFrameInventory(State* state);
void calcFrameProgramming(Player* player, State* state, uint32_t ticks);

#endif