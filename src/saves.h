#ifndef SAVES_H
#define SAVES_H

#include <stdbool.h>

#define SAVE_FOLDER           ".keycraft"

#define SAVE_VERSION          41

#define INSTANTPLAY_SAVE_NAME "instantplay.sav"

#define NUM_SAVES             3
#define SAVE_BASENAME         "game"
#define SAVE_EXTENSION        ".sav"
//Provision for 2 digits of save index
#define SAVE_NAME_LENGTH      strlen(SAVE_BASENAME) + 2 + strlen(SAVE_EXTENSION)

#define OPTIONS_SAVE_NAME     "options.sav"

extern bool gamesPresent[NUM_SAVES];
void getSaveNameForIndex(char* buffer, uint8_t index);
void checkGamesPresent();

#endif