#ifndef SAVES_H
#define SAVES_H

#include <stdbool.h>
#include <stdint.h>

#include "player.h"

#define SAVE_FOLDER ".keycraft"

//Save version: Last digit is minor (only additions, requires default values for old files),
//              Front digits are major (requires old loading logic to migrate)
#define SAVE_VERSION  41

typedef enum {
    SV_COMPAT_NONE = 0,
    SV_COMPAT_MAJOR,
    SV_COMPAT_MINOR,
    SV_COMPAT_OK
} SaveVersionCompat;

/**
 * Requires a save to be open
 * Must be the first read on that save
 */
SaveVersionCompat readSaveVersionCompat();

#define INSTANTPLAY_SAVE_NAME "instantplay.sav"

#define NUM_SAVES             3
#define SAVE_BASENAME         "game"
#define SAVE_EXTENSION        ".sav"
//Provision for 2 digits of save index
#define SAVE_NAME_LENGTH      strlen(SAVE_BASENAME) + 2 + strlen(SAVE_EXTENSION)

#define GAME_INDEX_NAME       "gameindex.sav"

#define OPTIONS_SAVE_NAME     "options.sav"

extern bool gamesPresent[NUM_SAVES];
void getSaveNameForIndex(char* buffer, uint8_t index);
void checkGamesPresent();

#define GAME_INDEX_NONE 255
void saveGameIndex(uint8_t gameIndex);
uint8_t loadGameIndex();

typedef enum {
    SR_OK,
    SR_FAIL_OPEN,
    SR_NOCHANGE
} SaveResult;

typedef enum {
    LR_OK,
    LR_FAIL_OPEN,
    LR_FAIL_SVC
} LoadResult;

SaveResult saveGame(char* name, Player* player);
LoadResult loadGame(char* name, Player* player);

#endif