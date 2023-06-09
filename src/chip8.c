#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "chip8.h"
#include "graphic.h"
#include "audio.h"

unsigned char memory[MEMORY_SIZE];
unsigned char gfx[PIXELS];
unsigned short stack[STACK_LEVELS];
unsigned char key[16];
bool drawFlag = false;

void (*Chip8Table[17])(chip8 *chip);

unsigned char fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void initChip(chip8 *chip) {
  chip->programCounter = 0x200;
  chip->opcode = 0;
  chip->index = 0;
  chip->stackPointer = 0;
  chip->state = RUNNING;

  // clearing memory
  memset(memory, 0, 0xFFF);
  memset(gfx, 0, PIXELS);
  memset(stack, 0, STACK_LEVELS);
  memset(key, 0, 16);
  memset(chip->V, 0, 16);

  for (int i = 0; i < 80; i++) {
    memory[i] = fontset[i];
  }
}

void fetch(chip8 *chip) {
    chip->opcode = 
        memory[chip->programCounter] << 8 | memory[chip->programCounter + 1];

    chip->programCounter += 2;
}

void execute(chip8 *chip) {
    fetch(chip);
    Chip8Table[(chip->opcode & 0xF000)>>12](chip);
}

void (*Chip8Table[17])() = {
    op0, op1, op2, op3, op4, op5, op6, op7,
    op8, op9, opA, opB, opC, opD, opE, opF
};

void op0(chip8 *chip) {
    switch (chip->opcode & 0x00F) {
        case 0x0000: // 0x00E0 - clears the screen
            memset(gfx, 0, PIXELS);
            drawFlag = true;
            break;
        case 0x000E: // 0x00EE - returns from subroutine
            --chip->stackPointer;
            chip->programCounter = stack[chip->stackPointer];
            break;
        default:
            printf("Unknown opcode [0x0000]: 0x%X\n", chip->opcode);
    }
}

// 1NNN: JUMPS TO ADDRESS NNN
void op1(chip8 *chip) {
    chip->programCounter = (chip->opcode & 0x0FFF);
}

// 2NNN: CALLS SUBROUTINE AT NNN
void op2(chip8 *chip) {
    stack[chip->stackPointer] = chip->programCounter;
    ++chip->stackPointer;
    chip->programCounter = (chip->opcode & 0x0FFF);
}

// 3XNN: SKIPS THE NEXT INSTRUCTION IF VX EQ NN
void op3(chip8 *chip) {
    if (chip->V[(chip->opcode & 0x0F00) >> 8] == (chip->opcode & 0x00FF)) {
        chip->programCounter += 2;
    }
}

// 4XNN: SKIPS THE NEXT INSTRUCTION IF VX NOT EQ NN
void op4(chip8 *chip) {
    if (chip->V[(chip->opcode & 0x0F00) >> 8] != (chip->opcode & 0x00FF)) {
        chip->programCounter += 2;
    }
}

// 5XY0: SKIPS THE NEXT INSTRUCTION IF VX EQ VY
void op5(chip8 *chip) {
    if (chip->V[(chip->opcode & 0x0F00) >> 8] == chip->V[(chip->opcode & 0x00F0) >> 4]) {
        chip->programCounter += 2;
    }
}

// 7XNN: SETS VX TO NN
void op6(chip8 *chip) {
    chip->V[(chip->opcode & 0x0F00) >> 8] = (chip->opcode & 0x00FF);    
}

// 8XY0: ADDS NN TO VS (W/O CARRY FLAG)
void op7(chip8 *chip) {
    chip->V[(chip->opcode & 0x0F00) >> 8] += (chip->opcode & 0x00FF);
}

void op8(chip8 *chip) {
    switch (chip->opcode & 0x000F) {
        case 0x0000: // 8XY0: sets VX to the value of VY
            chip->V[(chip->opcode & 0x0F00) >> 8] = chip->V[(chip->opcode & 0x00F0) >> 4];
            break;
        case 0x0001: // 8XY1: sets VX to (VX | VY)
            chip->V[(chip->opcode & 0x0F00) >> 8] |= chip->V[(chip->opcode & 0x00F0) >> 4];
            break;
        case 0x0002: // 8XY2: sets VX to (VX & VY)
            chip->V[(chip->opcode & 0x0F00) >> 8] &= chip->V[(chip->opcode & 0x00F0) >> 4];
            break;
        case 0x0003: // 8XY3: sets VX to (VX xor VY)
            chip->V[(chip->opcode & 0x0F00) >> 8] ^= chip->V[(chip->opcode & 0x00F0) >> 4];
            break;
        case 0x0004: // 8XY4: adds VY to VX (carry)
            if (chip->V[(chip->opcode & 0x00F0) >> 4] > (0xFF - chip->V[(chip->opcode & 0x0F00) >> 8])) {
                chip->V[0xF] = 1; // carry
            } else {
                chip->V[0xF] = 0;
            }
            
            chip->V[(chip->opcode & 0x0F00) >> 8] += chip->V[(chip->opcode & 0x00F0) >> 4];
            break;
        case 0x0005: // 8XY5: VY is substracted from VX (borrow)
            if (chip->V[(chip->opcode & 0x00F0) >> 4] > chip->V[(chip->opcode & 0x0F00) >> 8]) {
                chip->V[0xF] = 0; // borrow
            } else {
                chip->V[0xF] = 1;
            }

            chip->V[(chip->opcode & 0x0F00) >> 8] -= chip->V[(chip->opcode & 0x00F0) >> 4];
            break;
        case 0x0006: // 8XY6: Store the least significant bit of VX in VF then shifts VX to the right by 1
            chip->V[0xF] = chip->V[(chip->opcode & 0x0F00) >> 8] & 0x1;
            chip->V[(chip->opcode & 0x0F00) >> 8] >>= 1;
            break;
        case 0x0007: // 8XY7: Sets VX to VY - VX (borrow)
            if (chip->V[(chip->opcode & 0x0F00) >> 8] > chip->V[(chip->opcode & 0x00F0) >> 4]) {
                chip->V[0xF] = 0; // borrow
            } else {
                chip->V[0xF] = 1;
            }

            chip->V[(chip->opcode & 0x0F00) >> 8] = (chip->V[(chip->opcode & 0x00F0) >> 4] - chip->V[(chip->opcode & 0x0F00) >> 8]);
            break;
        case 0x000E: // 8XYE: Stores the most significant bit of VX in VF then shift VX to the left by 1
            chip->V[0xF] = chip->V[(chip->opcode & 0x0F00) >> 8] >> 7;
            chip->V[(chip->opcode & 0x0F00) >> 8] <<= 1;
            break;
        default:
            printf("Unknown opcode [0x0000]: 0x%X\n", chip->opcode);
    }
}

// 9XY0: SKIPS THE NEXT INSTRUCTION IF VX NOT EQ VY
void op9(chip8 *chip) {
    if (chip->V[(chip->opcode & 0x0F00) >> 8] != chip->V[(chip->opcode & 0x00F0) >> 4]) {
        chip->programCounter += 2;
    }
}

// ANNN: SETS INDEX TO THE ADDRESS NNN
void opA(chip8 *chip) {
    chip->index = chip->opcode & 0x0FFF;
}

// BNNN: JUMPS TO THE ADDRESS NNN PLUS V0
void opB(chip8 *chip) {
    chip->programCounter = ((chip->opcode & 0x0FFF) + chip->V[0]);
}

// CXNN: SETS VX TO THE RESULT OF BITWISE AND OPERATION ON RANDOM NUMBER AND NN
void opC(chip8 *chip) {
    chip->V[(chip->opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (chip->opcode & 0x00FF);
}

// DXYN: DRAWS SPRITE AT COORDINADE (VX, VY)
void opD(chip8 *chip) {
    unsigned short x = chip->V[(chip->opcode & 0x0F00) >> 8];
    unsigned short y = chip->V[(chip->opcode & 0x00F0) >> 4];
    unsigned short height = chip->opcode & 0x000F;
    unsigned short pixel;

    chip->V[0xF] = 0;
    for (int yline = 0; yline < height; yline++) {
        pixel = memory[chip->index + yline];
        for (int xline = 0; xline < 8; xline++) {
            if ((pixel & (0x80 >> xline)) !=0 ) {
                if (gfx[(x + xline + ((y + yline) * 64))] == 1) {
                    chip->V[0xF] = 1;
                }
                gfx[x + xline + ((y + yline) * 64)] ^= 1;
            }
        }
    }

    drawFlag = true;
}

void opE(chip8 *chip) {
    switch (chip->opcode & 0x00FF) {
    case 0x009E: // EX9E: skips to the next instruction if the key stored in VX is pressed
        if (key[chip->V[(chip->opcode & 0x0F00) >> 8]] != 0) {
            chip->programCounter += 2;
        }
        
        break;
    case 0x00A1: // EXA1: skips to the next if the key stored in VX is not pressed
        if (key[chip->V[(chip->opcode & 0x0F00) >> 8]] == 0) {
            chip->programCounter += 2;
        }

        break;
    default:
        printf("Unknown opcode [0x0000]: 0x%X\n", chip->opcode);
    }
}

void opF(chip8 *chip) {
    switch (chip->opcode & 0x00FF) {
        case 0x0007: // FX07: sets VX to the value of the delay timer
            chip->V[(chip->opcode & 0x0F00) >> 8] = chip->delayTimer;
            break;
        case 0x000A: // FX0A: A key press is awaited and then stored in VX
        {
            bool keyPressed = false;

            for (int i=0; i < 16; i++) {
                if (key[i] != 0) {
                    chip->V[(chip->opcode & 0x0F00) >> 8] = i;
                    keyPressed = true;
                }
            }

            if (!keyPressed) { return; }
            break;
        }
        case 0x0015: // FX15: Sets the delay timer to VX
            chip->delayTimer = chip->V[(chip->opcode & 0x0F00) >> 8];
            break;
        case 0x0018: // FX18: Sets the sound timer to VX
            chip->soundTimer = chip->V[(chip->opcode & 0x0F00) >> 8];
            break;
        case 0x001E: // FXIE: Adds VX to I, VF is not affected
            chip->index += chip->V[(chip->opcode & 0x0F00) >> 8];
            break;
        case 0x0029: // Sets I to the location of the sprite character in VX
            chip->index = chip->V[(chip->opcode & 0x0F00) >> 8] * 0x5;
            break;
        case 0x0033: // Stores the binary-coded decimal representation of VX, with the hundreds digits in memory
            memory[chip->index] = chip->V[(chip->opcode & 0x0F00) >> 8] / 100;
            memory[chip->index + 1] = (chip->V[(chip->opcode & 0x0F00) >> 8] / 10) % 10;
            memory[chip->index + 2] = (chip->V[(chip->opcode & 0x0F00) >> 8] % 100) % 10;
            break;
        case 0x0055: // Stores from V0 to VX in memory starting at address I
            for (int i = 0; i <= ((chip->opcode & 0x0F00) >> 8); i++) {
                memory[chip->index + i] = chip->V[i];
            }
            break;
        case 0x0065: // Fills from V0 to VX with values from memory starting at address I
            for (int i = 0; i <= ((chip->opcode & 0x0F00) >> 8); i++) {
                chip->V[i] = memory[chip->index + i];
            }
            break;
        default:
            printf("Unknown opcode [0x0000]: 0x%X\n", chip->opcode);
    }
}

void timerHandler(chip8 *chip) {
    if (chip->delayTimer > 0) {
        --chip->delayTimer;
    }

    if (chip->soundTimer > 0) {
        if (chip->soundTimer == 1) {
            playSound();
        }
        --chip->soundTimer;
    }
}

void emulateInstruction(chip8 *chip) {
    fetch(chip);
    Chip8Table[(chip->opcode & 0xF000) >> 12](chip);
    
    timerHandler(chip);
}

void loadGame(const char *filename) {
    FILE *file;
    file = fopen(filename, "rb");
    
    if (!file) {
        printf("File not found.\n");
        exit(64);
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);
    printf("File size: %ld bytes\n", fileSize);

    char *buffer = (char *)malloc(sizeof(char) * fileSize);
    if (!buffer) {
        printf("Failed allocation of buffer.\n");
        exit(64);
    }
    
    size_t r = fread(buffer, 1, fileSize, file);
    if (r != fileSize) {
        printf("Failed loading game to buffer.\n");
        exit(64);
    }

    if ((0xfff - 0x200) > fileSize) {
        for (int i = 0; i < fileSize; ++i) {
            memory[i + 512] = buffer[i];
        }

        printf("Game loaded successfully!\n");
    } else {
        printf("LOAD ERROR\n");
        exit(64);
    }
    
    fclose(file);
    free(buffer);
}
