#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "chip8.h"

SDL_Window* window;
SDL_Event event;
SDL_Renderer* renderer;

int initGraphics() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Yet Another Chip-8 Interpreter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGTH, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    return 0;
}

void loopGraphic() {
    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        //quit = true;
                        break;
                    case SDLK_1:
                        key[0x1] = 1;
                        break;
                    case SDLK_2:
                        key[0x2] = 1;
                        break;
                    case SDLK_3:
                        key[0x3] = 1;
                        break;
                    case SDLK_4:
                        key[0xC] = 1;
                        break;
                    case SDLK_q:
                        key[0x4] = 1;
                        break;
                    case SDLK_w:
                        key[0x5] = 1;
                        break;
                    case SDLK_e:
                        key[0x6] = 1;
                        break;
                    case SDLK_r:
                        key[0xD] = 1;
                        break;
                    case SDLK_a:
                        key[0x7] = 1;
                        break;
                    case SDLK_s:
                        key[0x8] = 1;
                        break;
                    case SDLK_d:
                        key[0x9] = 1;
                        break;
                    case SDLK_f:
                        key[0xE] = 1;
                        break;
                    case SDLK_z:
                        key[0xA] = 1;
                        break;
                    case SDLK_x:
                        key[0x0] = 1;
                        break;
                    case SDLK_c:
                        key[0xB] = 1;
                        break;
                    case SDLK_v:
                         key[0xF] = 1;
                        break;
                    default:
                        break;
                }
                break;      
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_1:
                        key[0x1] = 0;
                        break;
                    case SDLK_2:
                        key[0x2] = 0;
                        break;
                    case SDLK_3:
                        key[0x3] = 0;
                        break;
                    case SDLK_4:
                        key[0xC] = 0;
                        break;
                    case SDLK_q:
                        key[0x4] = 0;
                        break;
                    case SDLK_w:
                        key[0x5] = 0;
                        break;
                    case SDLK_e:
                        key[0x6] = 0;
                        break;
                    case SDLK_r:
                        key[0xD] = 0;
                        break;
                    case SDLK_a:
                        key[0x7] = 0;
                        break;
                    case SDLK_s:
                        key[0x8] = 0;
                        break;
                    case SDLK_d:
                        key[0x9] = 0;
                        break;
                    case SDLK_f:
                        key[0xE] = 0;
                        break;
                    case SDLK_z:
                        key[0xA] = 0;
                        break;
                    case SDLK_x:
                        key[0x0] = 0;
                        break;
                    case SDLK_c:
                        key[0xB] = 0;
                        break;
                    case SDLK_v:
                        key[0xF] = 0;
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }
}

void drawGraphic() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);

    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            if (gfx[(y * 64) + x] == 1) {
                SDL_Rect rect = { x * 10, y * 10, 10, 10 };
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void cleanupSdl() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
