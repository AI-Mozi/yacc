#ifndef CHIP_H
#define CHIP_H

#include <stdbool.h>

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

#endif
