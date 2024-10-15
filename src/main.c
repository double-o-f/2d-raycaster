#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#define SCREEN_WIDTH  80 // 1440// 
#define SCREEN_HEIGHT 60 // 1080// 


uint32_t pixels[SCREEN_WIDTH * SCREEN_WIDTH];

SDL_Window *window;
SDL_Texture *texture;
SDL_Renderer *renderer;

bool quit = false;

struct {
    int x;
    int y;
} p;



uint32_t setColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A) {
    uint32_t col = (16777216 * R) + (65536 * G) + (256 * B) + (1 * A);
    return col;
}


int main(int argc, char const *argv[])
{
    fprintf(stdout, "%s\n", ":)");

    if(SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL failed to initialize: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow(
        "test", 
        SDL_WINDOWPOS_CENTERED_DISPLAY(0), 
        SDL_WINDOWPOS_CENTERED_DISPLAY(0), 
        800, 
        600, 
        SDL_WINDOW_ALLOW_HIGHDPI);
    if(!window) {
        fprintf(stderr, "failed to create SDL window: %s\n", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if(!renderer) {
        fprintf(stderr, "failed to create SDL renderer: %s\n", SDL_GetError());
    }

    texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            SCREEN_WIDTH,
            SCREEN_HEIGHT);
    if(!texture) {
        fprintf(stderr, "failed to create SDL texture: %s\n", SDL_GetError());
    }

    p.x = 0;
    p.y = 0;


    while (!quit){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        const uint8_t *keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_LEFT]) {
            p.x -= 1;
        }
        if (keystate[SDL_SCANCODE_RIGHT]) {
            p.x += 1;
        }
        if (keystate[SDL_SCANCODE_UP]) {
            p.y -= 1;
        }
        if (keystate[SDL_SCANCODE_DOWN]) {
            p.y += 1;
        }


        memset(pixels, 0, sizeof(pixels));

        if (p.x < SCREEN_WIDTH && p.x >= 0 && p.y < SCREEN_HEIGHT && p.y >= 0)
        {
            pixels[p.x + (p.y * SCREEN_WIDTH)] = setColor((p.x * 255) / SCREEN_WIDTH, 255 - ((p.y * 255) / SCREEN_HEIGHT), (p.y * 255) / SCREEN_HEIGHT, 255);
        }
        //delete these later
        if (p.x + 1 < SCREEN_WIDTH && p.x + 1 >= 0 && p.y < SCREEN_HEIGHT && p.y >= 0)
        {
            pixels[p.x + 1 + (p.y * SCREEN_WIDTH)] = setColor((p.x * 255) / SCREEN_WIDTH, 255 - ((p.y * 255) / SCREEN_HEIGHT), (p.y * 255) / SCREEN_HEIGHT, 255);
        }
        if (p.x < SCREEN_WIDTH && p.x >= 0 && p.y + 1 < SCREEN_HEIGHT && p.y + 1 >= 0)
        {
            pixels[p.x + ((p.y + 1) * SCREEN_WIDTH)] = setColor((p.x * 255) / SCREEN_WIDTH, 255 - ((p.y * 255) / SCREEN_HEIGHT), (p.y * 255) / SCREEN_HEIGHT, 255);
        }
        if (p.x + 1 < SCREEN_WIDTH && p.x + 1 >= 0 && p.y + 1 < SCREEN_HEIGHT && p.y + 1 >= 0)
        {
            pixels[p.x + 1 + ((p.y + 1) * SCREEN_WIDTH)] = setColor((p.x * 255) / SCREEN_WIDTH, 255 - ((p.y * 255) / SCREEN_HEIGHT), (p.y * 255) / SCREEN_HEIGHT, 255);
        }


        SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * 4);
        SDL_RenderCopyEx(
            renderer,
            texture,
            NULL,
            NULL,
            0.0,
            NULL,
            0); //SDL_FLIP_VERTICAL
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    exit(EXIT_SUCCESS);
}
