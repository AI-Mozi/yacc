#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"
#include "graphic.h"

chip8 chip;

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: ./yacc [file_path]\n");
    exit(64);
  }

  if (initGraphics() != 0) {
    return 1;
  }

  initAudio();

  initChip(&chip);
  loadGame(argv[1]);

  for (;;) {
    emulateCycle(&chip);

    if (drawFlag) {
      drawGraphic();
    }

    loopGraphic();

    if (quit) {
      return 1;
    }
  }
}
