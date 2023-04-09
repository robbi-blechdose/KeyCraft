#ifndef PROGRAMMING_H
#define PROGRAMMING_H

#include <stdbool.h>

#include "../computer.h"

void drawProgrammingScreen(ComputerData* computer);

void moveProgrammingCursor(int8_t dirX, int8_t dirY);
void enterProgrammingCursor(ComputerData* computer);
bool cancelProgrammingCursor();

void shiftProgramDown(ComputerData* computer);
void shiftProgramUp(ComputerData* computer);

#endif