#ifndef _VERSION_H
#define _VERSION_H

#define GAME_BASE_VERSION "1.0"

#ifdef DEBUG
#define GAME_VERSION GAME_BASE_VERSION "-debug"
#else
#define GAME_VERSION GAME_BASE_VERSION
#endif

#endif