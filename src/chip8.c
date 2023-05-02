#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8.h"

unsigned char memory[MEMORY_SIZE];
unsigned char gfx[PIXELS];
unsigned short stack[STACK_LEVELS];
unsigned char key[16];

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

void initialize(chip8 *chip) {
  chip->programCounter = 0x200;
  chip->opcode = 0;
  chip->index = 0;
  chip->stackPointer = 0;

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

void emulateCycle(chip8 *chip) {
  chip->opcode =
      memory[chip->programCounter] << 8 | memory[chip->programCounter + 1];

  switch (chip->opcode & 0xF000) {
    case 0x0000:
        switch (chip->opcode & 0x00F) {
            case 0x0000: // 0x00E0 - clears the screen
                break;
            case 0x000E: // 0x00EE - returns from subroutine
                break;
            default:
                printf("Unknown opcode [0x0000]: 0x%X\n", chip->opcode);
        }
        break;
    case 0x1000: // jumps to address 
        break;
    case 0x2000: // calls subroutine
        break;
    case 0x3000: // 3XNN: skips the next instruction if Vx == NN
        break;
    case 0x4000: // 4XNN: skips the next instruction if Vx != NN
        break;
    case 0x5000: // 5XY0: skips the next instruction if Vx == Vy
        break;
    case 0x6000: // 6XNN: sets VX to NN
        break;
    case 0x7000: // 7XNN: adds NN to VX
        break;
    case 0x8000:
        switch (chip->opcode & 0x000F) {
            case 0x0000: // 8XY0: sets VX to the value of VY
                break;
            case 0x0001: // 8XY1: sets VX to (VX | VY)
                break;
            case 0x0002: // 8XY2: sets VX to (VX & VY)
                break;
            case 0x0003: // 8XY3: sets VX to (VX xor VY)
                break;
            case 0x0004: // 8XY4: adds VY to VX (carry)
                break;
            case 0x0005: // 8XY5: VY is substracted from VX (borrow)
                break;
            case 0x0006: // 8XY6: Store the least significant bit of VX in VF then shifts VX to the right by 1
                break;
            case 0x0007: // 8XY7: Sets VX to VY - VX (borrow)
                break;
            case 0x000E: // 8XYE: Stores the most significant bit of VX in VF then sgift VX to the left by 1
                break;
            default:
                printf("Unknown opcode [0x0000]: 0x%X\n", chip->opcode);
        }
        break;
    case 0x9000: // 9XY0: Sets I to the addres NNN
        break;
    case 0xA000:
        chip->index = chip->opcode & 0xFFF;
        chip->programCounter += 2;
    case 0xB000: // BNNN: Jumps to thje address NNN plus V0
        break;
    case 0xC000: // CXNN: Sets VX to the result of a bitwise and operation on rundom number and NN
        break;
    case 0xD000: // DXYN: draws sprite at coordinate (VX, VY)
        break;
    case 0xE000:
        switch (chip->opcode & 0x000F) {
            case 0x000E: // EX9E: skips to the next instruction if the key stored in VX is pressed
                break;
            case 0x0001: // EXA1: skips to the next if the key stored in VX is not pressed
                break;
            default:
                printf("Unknown opcode [0x0000]: 0x%X\n", chip->opcode);
        }
        break;
    case 0xF000:
        switch (chip->opcode & 0x000F) {
            case 0x0007: // FX07: sets VX to the value of the delay timer
                break;
            case 0x000A: // FX0A: A key press is awaited and then stored in VX
                break;
            case 0x0005:
                switch (chip->opcode & 0x00F0) {
                    case 0x0010: // FX15: sets the delay timer to VX
                        break;
                    case 0x0050: // FX55: stores from V0 to VX in memory, starting at address I
                        break;
                    case 0x0060: // FX65: fills from V0 to VX with values from memory, startind at address I
                        break;
                    default:
                        printf("Unknown opcode [0x0000]: 0x%X\n", chip->opcode);
                }
                break;
            case 0x0008: // Sets the sound timer to VX
                break;
            case 0x000E: // Adds VX to I, VF is not affected
                break;
            case 0x0009: // Sets I to the location of the sprite character in VX
                break;
            case 0x0003: // Stores the binary-coded decimal representation of VX, with the hundreds digits in memory
                break;
            default:
                printf("Unknown opcode [0x0000]: 0x%X\n", chip->opcode);
        }
        break;
    default:
        printf("Unknown OpCode: 0x%X\n", chip->opcode);
  }

  if (chip->delayTimer > 0) {
    --chip->delayTimer;
  }

  if (chip->soundTimer > 0) {
    if (chip->soundTimer == 1) {
      printf("BEEP!\n");
      --chip->soundTimer;
    }
  }
}

void loadGame(const char *filename) {
    FILE * file;

    file = fopen(filename, "rb");
    if (!file) {
        printf("Failed loading a game.");
        exit(74);
    }

    printf("Game loaded correctly!");

    
    fclose(file);
}
