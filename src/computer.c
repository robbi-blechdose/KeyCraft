#include "computer.h"

#include <stdlib.h>

#include "engine/savegame.h"

ComputerData* createComputer()
{
    ComputerData* computer = malloc(sizeof(ComputerData));
    computer->af = 0;
    computer->pc = 0;
    computer->io = 0;
    for(uint8_t i = 0; i < RAM_SIZE; i++)
    {
        computer->ram[i] = 0;
    }
    for(uint8_t i = 0; i < PROGRAM_SIZE; i++)
    {
        computer->program[i] = 0;
    }
    return computer;
}

void writeToRAM(ComputerData* computer, uint8_t location, uint8_t data)
{
    if(location % 2 == 0)
    {
        computer->ram[location / 2] = TO_HIGH_NIBBLE(data) | LOW_NIBBLE(computer->ram[location / 2]);
    }
    else
    {
        computer->ram[location / 2] = (computer->ram[location / 2] & 0xF0) | LOW_NIBBLE(data);
    }
}

uint8_t readFromRAM(ComputerData* computer, uint8_t location)
{
    return location % 2 == 0 ? HIGH_NIBBLE(computer->ram[location / 2]) : LOW_NIBBLE(computer->ram[location / 2]);
}

void setA(ComputerData* computer, uint8_t data)
{
    computer->af = LOW_NIBBLE(computer->af) | TO_HIGH_NIBBLE(data);
}

void setFlag(ComputerData* computer, uint8_t flag)
{
    computer->af |= flag;
}

void clearFlag(ComputerData* computer, uint8_t flag)
{
    computer->af &= ~flag;
}

//Sets or clears a flag according to the result of an operation
void setOrClearFlagZ(ComputerData* computer, uint8_t result)
{
    if(result == 0)
    {
        setFlag(computer, COMPUTER_FLAG_ZERO);
    }
    else
    {
        clearFlag(computer, COMPUTER_FLAG_ZERO);
    }
}

bool isFlagSet(ComputerData* computer, uint8_t flag)
{
    return computer->af & flag;
}

void runComputerCycle(ComputerData* computer)
{
    //Get instruction
    Instruction instruction = HIGH_NIBBLE(computer->program[computer->pc]);
    uint8_t data = LOW_NIBBLE(computer->program[computer->pc]);
    switch(instruction)
    {
        case NOP:
        {
            break;
        }
        case STA:
        {
            writeToRAM(computer, data, HIGH_NIBBLE(computer->af));
            break;
        }
        case LDA:
        {
            setA(computer, readFromRAM(computer, data));
            break;
        }
        case ADM:
        {
            //LOW_NIBBLE() to keep it within 4 bits
            uint8_t result = LOW_NIBBLE(HIGH_NIBBLE(computer->af) + readFromRAM(computer, data));
            setA(computer, result);
            setOrClearFlagZ(computer, result);
            break;
        }
        case SBM:
        {
            //LOW_NIBBLE() to keep it within 4 bits
            uint8_t result = LOW_NIBBLE(HIGH_NIBBLE(computer->af) - readFromRAM(computer, data));
            setA(computer, result);
            setOrClearFlagZ(computer, result);
            break;
        }
        case IMA:
        {
            setA(computer, data);
            break;
        }
        case ADI:
        {
            //LOW_NIBBLE() to keep it within 4 bits
            uint8_t result = LOW_NIBBLE(HIGH_NIBBLE(computer->af) + data);
            setA(computer, result);
            setOrClearFlagZ(computer, result);
            break;
        }
        case SBI:
        {
            //LOW_NIBBLE() to keep it within 4 bits
            uint8_t result = LOW_NIBBLE(HIGH_NIBBLE(computer->af) - data);
            setA(computer, result);
            setOrClearFlagZ(computer, result);
            break;
        }
        case JP:
        {
            computer->pc = computer->program[computer->pc + 1];
            break;
        }
        case JPZ:
        {
            if(isFlagSet(computer, COMPUTER_FLAG_ZERO))
            {
                computer->pc = computer->program[computer->pc + 1];
            }
            else
            {
                computer->pc += 2;
            }
            break;
        }
        case JPN:
        {
            if(!isFlagSet(computer, COMPUTER_FLAG_ZERO))
            {
                computer->pc = computer->program[computer->pc + 1];
            }
            else
            {
                computer->pc += 2;
            }
            break;
        }
        case IN:
        {
            setA(computer, HIGH_NIBBLE(computer->io) & data);
            break;
        }
        case OUT:
        {
            computer->io = (computer->io & 0xF0) | (HIGH_NIBBLE(computer->af) & data) | (LOW_NIBBLE(computer->io) & ~data);
            break;
        }
    }
    
    if(instruction != JP && instruction != JPZ && instruction != JPN)
    {
        computer->pc++;
    }
    if(computer->pc >= PROGRAM_SIZE)
    {
        computer->pc = 0;
    }
}

/**
void printComputerState(ComputerData* computer)
{
    printf("+----------------+\n");
    printf("|AF: %02x PC: %04x |\n", computer->af, computer->pc);
    printf("|");
    for(uint8_t i = 0; i < 8; i++)
    {
        printf("%02x", computer->ram[i]);
    }
    printf("|\n");
    printf("|");
    for(uint8_t i = 8; i < 16; i++)
    {
        printf("%02x", computer->ram[i]);
    }
    printf("|\n");
    printf("+----------------+\n");
}**/

void saveComputer(ComputerData* computer)
{
    writeElement(&computer->program, PROGRAM_SIZE * sizeof(uint8_t));
    writeElement(&computer->ram, RAM_SIZE * sizeof(uint8_t));
    writeElement(&computer->pc, sizeof(uint8_t));
    writeElement(&computer->af, sizeof(uint8_t));
    writeElement(&computer->io, sizeof(uint8_t));
}

void loadComputer(ComputerData* computer)
{
    readElement(&computer->program, PROGRAM_SIZE * sizeof(uint8_t));
    readElement(&computer->ram, RAM_SIZE * sizeof(uint8_t));
    readElement(&computer->pc, sizeof(uint8_t));
    readElement(&computer->af, sizeof(uint8_t));
    readElement(&computer->io, sizeof(uint8_t));
}