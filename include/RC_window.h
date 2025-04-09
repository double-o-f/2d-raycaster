#ifndef HEADER_RC_WI_INCLUDED
#define HEADER_RC_WI_INCLUDED

#include <stdio.h> //printf, fprintf

#ifdef __WIN64
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>

#include "RC_rend.h" //RD_rend.pixels, RD_rend.screenWidth, RD_rend.screenHeight


typedef struct {
    uint16_t windowWidth; 
    uint16_t windowHeight; 

    SDL_Window* window;
    SDL_Texture* texture;
    SDL_Renderer* renderer;
} WI_window_;

extern WI_window_ WI_window;


void WI_draw();

void WI_destroy();


int WI_init();

#endif
