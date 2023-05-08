#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"
#include "graphic.h"
#include <SDL2/SDL.h>

chip8 chip;

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: ./yacc [file_path]\n");
    exit(64);
  }

  if (initGraphics() != 0) { return 1; }

  initAudio();

  initChip(&chip);
  loadGame(argv[1]);

  while (chip.state != QUIT) {
    inputHandle(&chip);
    
    if (chip.state == PAUSED) { continue; }
    
    const uint64_t startFrameTime = SDL_GetPerformanceCounter();

    for (uint32_t i = 0; i < (INSTRUCTIONS_PER_SECOND / 60); i++) { emulateInstruction(&chip); }

    const uint64_t endFrameTime = SDL_GetPerformanceCounter();

    const double timeElapsed = (double)((endFrameTime - startFrameTime) * 1000) /
        SDL_GetPerformanceCounter();

    SDL_Delay(16.67f > timeElapsed ? 16.67f - timeElapsed : 0);

    if (drawFlag) {
        drawGraphic();
        drawFlag = false;
    }
  }

  cleanupSdl();
  exit(EXIT_SUCCESS);
}
