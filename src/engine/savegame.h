#ifndef SAVEGAME_H
#define SAVEGAME_H

#include <stddef.h>
#include <stdbool.h>

bool openSave(char* folder, char* name, bool writing);
void writeElement(void* ptr, size_t size);
void readElement(void* ptr, size_t size);
void closeSave();

#endif