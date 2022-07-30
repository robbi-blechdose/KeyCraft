#ifndef _COMPUTER_H
#define _COMPUTER_H

#include <stdint.h>

#define PROGRAM_SIZE 128
#define RAM_SIZE 16

typedef struct {
    uint8_t program[PROGRAM_SIZE];
    uint8_t ram[RAM_SIZE];
    uint8_t pc;
    uint8_t af;
    uint8_t io;
} ComputerData;

typedef enum {
    NOP = 0, //No OPeration
    STA, //STore A (to RAM)
    LDA, //LoaD A (from RAM)
    ADM, //ADd Memory (to A)
    SBM, //SuBtract Memory (from A)
    IMA, //load IMmediate (to A)
    ADI, //ADd Immediate (to A)
    SBI, //SuBtract Immediate (from A)
    JP,  //JumP (to address)
    JPZ, //JumP if Zero (to address)
    JPN, //JumP if Not zero (to address)
    IN,  //INput from side (to A)
    OUT  //OUTput to side (from A)
} Instruction;

#define HIGH_NIBBLE(X) ((X) >> 4)
#define TO_HIGH_NIBBLE(X) ((X) << 4)
#define LOW_NIBBLE(X) ((X) & 0x0F)
#define INSTRUCTION(X, Y) (TO_HIGH_NIBBLE(X) | LOW_NIBBLE(Y))

#define COMPUTER_FLAG_RUNNING 0b0001

ComputerData* createComputer();
void runComputerCycle(ComputerData* computer);

void saveComputer(ComputerData* computer);
void loadComputer(ComputerData* computer);

#endif