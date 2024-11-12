#include <stdio.h> //printf, fprintf

#ifdef __WIN64
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>

#include "RD_rend.h" //RD_rend.pixels, RD_rend.screenWidth, RD_rend.screenHeight

#include "WI_window.h"
struct WI_window_ WI_window;


void WI_draw() {
    SDL_UpdateTexture(WI_window.texture, NULL, RD_rend.pixels, RD_rend.screenWidth * 4);
    SDL_RenderCopyEx(
        WI_window.renderer,
        WI_window.texture,
        NULL,
        NULL,
        0.0,
        NULL,
        0); //SDL_FLIP_VERTICAL
    SDL_RenderPresent(WI_window.renderer);
}

void WI_destroy() {
    SDL_DestroyTexture(WI_window.texture);
    SDL_DestroyRenderer(WI_window.renderer);
    SDL_DestroyWindow(WI_window.window);
}


int WI_init() {
    WI_window.windowWidth = 1920; // 1440// 1600// 800
    WI_window.windowHeight = 1080; // 1080// 900// 450

    if(SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL failed to initialize: %s\n", SDL_GetError());
        return 1;
    }

    WI_window.window = SDL_CreateWindow(
        "ray", 
        SDL_WINDOWPOS_CENTERED_DISPLAY(0), 
        SDL_WINDOWPOS_CENTERED_DISPLAY(0), 
        WI_window.windowWidth, 
        WI_window.windowHeight,
        SDL_WINDOW_ALLOW_HIGHDPI); // | SDL_WINDOW_FULLSCREEN
    if(!WI_window.window) {
        fprintf(stderr, "failed to create SDL window: %s\n", SDL_GetError());
        return 1;
    }

    WI_window.renderer = SDL_CreateRenderer(WI_window.window, -1, SDL_RENDERER_PRESENTVSYNC); //SDL_RENDERER_PRESENTVSYNC
    if(!WI_window.renderer) {
        fprintf(stderr, "failed to create SDL renderer: %s\n", SDL_GetError());
        return 1;
    }

    WI_window.texture = SDL_CreateTexture(
            WI_window.renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            RD_rend.screenWidth,
            RD_rend.screenHeight);
    if(!WI_window.texture) {
        fprintf(stderr, "failed to create SDL texture: %s\n", SDL_GetError());
        return 1;
    }

    return 0;
}
