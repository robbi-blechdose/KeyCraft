#include "programming.h"

#include <stdbool.h>
#include <GL/gl.h>

#include "../engine/video.h"
#include "../engine/image.h"

#define UIBH 20

/**
 * Macro for centering text
 * X is the length of the text
 * Note: X * 8 / 2 = X * 4
 **/
#define CENTER(X) (WINX / 2 - (X) * 4)

#define TEXT_PROGRAM          0x7BCE00
#define TEXT_PROGRAM_SELECTED 0xEBDF00

const char* keyboard[] = {
    "STA ADM ADI IMA JPZ IN ",
    "LDA SBM SBI JP  JPN OUT",
    "NOP 0123456789  ABCDEF "
};

typedef struct {
    uint8_t x;
    char* str;
    uint8_t instruction;
} KeyboardEntry;

const KeyboardEntry keyboardChars[] = { 
    {0, "STA", STA},
    {4, "ADM", ADM},
    {8, "ADI", ADI},
    {12, "IMA", IMA},
    {16, "JPZ", JPZ},
    {20, "IN", IN},

    {0, "LDA", LDA},
    {4, "SBM", SBM},
    {8, "SBI", SBI},
    {12, "JP", JP},
    {16, "JPN", JPN},
    {20, "OUT", OUT},

    {0, "NOP", NOP},
    {4, "0", 0},
    {5, "1", 1},
    {6, "2", 2},
    {7, "3", 3},
    {8, "4", 4},
    {9, "5", 5},
    {10, "6", 6},
    {11, "7", 7},
    {12, "8", 8},
    {13, "9", 9},
    {16, "A", 10},
    {17, "B", 11},
    {18, "C", 12},
    {19, "D", 13},
    {20, "E", 14},
    {21, "F", 15}
};

const char* instructionLabels[] = {
    "NOP",
    "STA", "LDA",
    "ADM", "SBM",
    "IMA",
    "ADI", "SBI",
    "JP", "JPZ", "JPN",
    "IN", "OUT"
};

bool keyboardActive = false;
int16_t programCursor = 0;
int8_t cursorX = 0;
int8_t cursorY = 0;

bool isPreviousInstructionJump(ComputerData* computer, uint8_t index)
{
    if(index == 0)
    {
        return false;
    }

    uint8_t prevInstruction = HIGH_NIBBLE(computer->program[index - 1]);
    return prevInstruction == JP || prevInstruction == JPZ || prevInstruction == JPN;
}

void drawProgrammingScreen(ComputerData* computer)
{
    //Draw screen
    drawTexQuad(24, 64, 192, 160, UIBH, PTC(128), PTC(0), PTC(128 + 95), PTC(79));

    //Draw program
    char buffer[25];
    uint8_t programScroll = programCursor < PROGRAM_SIZE - 15 ? programCursor : PROGRAM_SIZE - 15;
    for(uint8_t i = 0; i < 15; i++)
    {
        uint8_t instruction = HIGH_NIBBLE(computer->program[programScroll + i]);

        if(isPreviousInstructionJump(computer, programScroll + i))
        {
            //If the previous instruction was a jump, this is an address
            sprintf(buffer, " %3d: %02X", programScroll + i, computer->program[programScroll + i]);
        }
        else
        {
            sprintf(buffer, " %3d: %s", programScroll + i, instructionLabels[instruction]);

            if(instruction != NOP && instruction != JP && instruction != JPZ && instruction != JPN)
            {
                //Print instruction operand if required
                sprintf(buffer + strlen(buffer), " %01X", LOW_NIBBLE(computer->program[programScroll + i]));
            }
        }

        //Print PC cursor
        if(computer->pc == programScroll + i)
        {
            buffer[0] = '>';
        }

        if(!keyboardActive && programScroll + i == programCursor)
        {
            glDrawText(buffer, 28, 22 + i * 8, TEXT_PROGRAM_SELECTED);
        }
        else
        {
            glDrawText(buffer, 28, 22 + i * 8, TEXT_PROGRAM);
        }
    }

    //Draw machine state
    glDrawText("State:", 122, 22, TEXT_PROGRAM);
    sprintf(buffer, "A: %01X", HIGH_NIBBLE(computer->af));
    glDrawText(buffer, 122, 38, TEXT_PROGRAM);
    sprintf(buffer, "PC: %04X", computer->pc);
    glDrawText(buffer, 122, 54, TEXT_PROGRAM);
    glDrawText("RAM:", 122, 70, TEXT_PROGRAM);
    for(uint8_t i = 0; i < RAM_SIZE / 4; i++)
    {
        buffer[0] = '\0';
        for(uint8_t j = 0; j < 4; j++)
        {
            sprintf(buffer + strlen(buffer), "%02X ", computer->ram[i * 4 + j]);
        }
        glDrawText(buffer, 122, 78 + i * 8, TEXT_PROGRAM);
    }

    sprintf(buffer, "I/O: %01X %01X", computer->io & 0x70, computer->io & 0x07);
    glDrawText(buffer, 122, 102, TEXT_PROGRAM);

    sprintf(buffer, "Running: %d", LOW_NIBBLE(computer->af));
    glDrawText(buffer, 122, 118, TEXT_PROGRAM);

    //Draw instruction keyboard
    for(uint8_t i = 0; i < 3; i++)
    {
        glDrawText(keyboard[i], 28, 149 + i * 8, TEXT_WHITE);
    }

    if(keyboardActive)
    {
        KeyboardEntry selection = keyboardChars[cursorX + cursorY * 6];
        glDrawText(selection.str, 28 + selection.x * 8, 149 + cursorY * 8, TEXT_PROGRAM_SELECTED);
    }
}

void moveProgrammingCursor(int8_t dirX, int8_t dirY)
{
    if(keyboardActive)
    {
        //Move keyboard cursor
        cursorX += dirX;
        if(cursorX < 0)
        {
            cursorX = 5;
            if(cursorY == 2)
            {
                cursorX = 16;
            }
        }
        else if(cursorY < 2 && cursorX > 5 || cursorY == 2 && cursorX > 16)
        {
            cursorX = 0;
        }

        cursorY += dirY;
        if(cursorY < 0)
        {
            cursorY = 2;
        }
        else if(cursorY > 2)
        {
            cursorY = 0;
        }
    }
    else
    {
        //Move program cursor
        programCursor += dirY;
        if(programCursor < 0)
        {
            programCursor = PROGRAM_SIZE - 1;
        }
        else if(programCursor >= PROGRAM_SIZE)
        {
            programCursor = 0;
        }
    }
}

void enterProgrammingCursor(ComputerData* computer)
{
    if(keyboardActive)
    {
        if(isPreviousInstructionJump(computer, programCursor))
        {
            //This is the address to jump to, so we need to edit a full byte
            if(cursorY == 2 && cursorX > 0)
            {
                computer->program[programCursor] = TO_HIGH_NIBBLE(LOW_NIBBLE(computer->program[programCursor]));
                computer->program[programCursor] |= LOW_NIBBLE(keyboardChars[cursorX + cursorY * 6].instruction);
            }
        }

        if(cursorY == 2 && cursorX > 0)
        {
            //Number entry
            computer->program[programCursor] |= LOW_NIBBLE(keyboardChars[cursorX + cursorY * 6].instruction);
        }
        else
        {
            //Instruction entry
            computer->program[programCursor] = TO_HIGH_NIBBLE(keyboardChars[cursorX + cursorY * 6].instruction);
        }
    }
    else
    {
        keyboardActive = true;
        computer->program[programCursor] = 0;
    }
}

//Returns true if cursor was cancelled
bool cancelProgrammingCursor()
{
    if(!keyboardActive)
    {
        return false;
    }

    keyboardActive = false;
    return true;
}

void shiftProgramDown(ComputerData* computer)
{
    if(keyboardActive)
    {
        return;
    }

    //Find end of the program
    uint8_t programEnd = PROGRAM_SIZE - 1;
    while(computer->program[programEnd] == NOP)
    {
        programEnd--;
    }

    if(programEnd == PROGRAM_SIZE - 1)
    {
        //Can't shift, no more space
        return;
    }

    for(uint8_t i = programEnd + 1; i > programCursor; i--)
    {
        computer->program[i] = computer->program[i - 1];
    }
    computer->program[programCursor] = NOP;
}