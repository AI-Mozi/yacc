#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "chip8.h"

int initGraphics();
void initAudio();
void drawGraphic();
void inputHandle(chip8 *chip);
void cleanupSdl();
void playAudio();

#endif
