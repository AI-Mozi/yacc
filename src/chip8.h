#ifndef CHIP_H
#define CHIP_H

#include <stdbool.h>
#include <stdint.h>

#define MEMORY_SIZE 4096
#define PIXELS 64 * 32
#define STACK_LEVELS 16
#define WIDTH 640
#define HEIGTH 320

typedef struct {
    unsigned char V[16];
    unsigned short opcode;
    unsigned short index;
    unsigned short programCounter;
    unsigned short stackPointer;
    unsigned char delayTimer;
    unsigned char soundTimer;
} chip8;

extern unsigned char memory[MEMORY_SIZE];
extern unsigned char gfx[PIXELS];
extern unsigned short stack[STACK_LEVELS];
extern unsigned char key[16];
extern bool drawFlag;
extern bool quit;

void initChip(chip8 *chip);
void emulateCycle(chip8 *chip);
void loadGame(const char* filename);
void op0(chip8 *chip);
void op1(chip8 *chip);
void op2(chip8 *chip);
void op3(chip8 *chip);
void op4(chip8 *chip);
void op5(chip8 *chip);
void op6(chip8 *chip);
void op7(chip8 *chip);
void op8(chip8 *chip);
void op9(chip8 *chip);
void opA(chip8 *chip);
void opB(chip8 *chip);
void opC(chip8 *chip);
void opD(chip8 *chip);
void opE(chip8 *chip);
void opF(chip8 *chip);

#endif
