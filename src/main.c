#include <stdlib.h>
#include <stdio.h>

#include "chip8.h"

chip8 myChip8;

int main(int argc, char**argv) {
    // Setup graphic
    // setupGraphic();
    if (argc != 2) {
        printf("usage: ./yacc [file_path]\n");
        exit(64);
    }

    initialize(&myChip8);
    loadGame(argv[1]);

    for (;;) {
        emulateCycle(&myChip8);

        // if (myChip8) {
        //  drawGraphic();
        //}

        // setKeys(&myChip8);
    }
}

