#ifndef _PROGRAMMING_H
#define _PROGRAMMING_H

#include "../computer.h"

void drawProgrammingScreen(ComputerData* computer);
void moveProgrammingCursor(int8_t dirX, int8_t dirY);
void enterProgrammingCursor(ComputerData* computer);
void cancelProgrammingCursor();

#endif