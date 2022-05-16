#include "savegame.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

FILE* saveFile;

uint8_t openSave(char* folder, char* name, uint8_t writing)
{
    char buffer[255];
    sprintf(buffer, "%s/%s/", getenv("HOME"), folder);
    //Check if we have to create the folder
    if(access(buffer, F_OK) == -1)
    {
        mkdir(buffer, 0755);
    }
    strcat(buffer, name);
    if(writing)
    {
        saveFile = fopen(buffer, "w+b");
    }
    else
    {
        saveFile = fopen(buffer, "rb");
    }

    if(saveFile == NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void writeElement(void* ptr, size_t size)
{
    fwrite(ptr, size, 1, saveFile);
}

void readElement(void* ptr, size_t size)
{
    fread(ptr, size, 1, saveFile);
}

void closeSave()
{
    if(saveFile != NULL)
    {
        fclose(saveFile);
    }
}