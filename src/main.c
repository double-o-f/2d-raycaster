#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __WIN64
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>

#define SCREEN_WIDTH  400 // 1440// 
#define SCREEN_HEIGHT 300 // 1080// 60

//#define MAP_SIZE 6
//int map[MAP_SIZE * MAP_SIZE + 1] = {
//            0, 1, 0, 1, 0, 1,
//            1, 0, 0, 0, 0, 1,
//            1, 0, 0, 0, 0, 1,
//            1, 0, 0, 0, 0, 1,
//            1, 0, 0, 0, 0, 1,
//            1, 1, 1, 1, 1, 1};

#define MAP_SIZE 7
int map[MAP_SIZE * MAP_SIZE] = {
            1, 1, 1, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 1,
            1, 0, 1, 0, 1, 0, 1,
            1, 0, 0, 0, 0, 0, 1,
            1, 0, 1, 0, 1, 0, 1,
            1, 0, 0, 0, 0, 0, 1,
            1, 1, 1, 1, 1, 1, 1};

//#define MAP_SIZE 8
//int map[MAP_SIZE * MAP_SIZE] = {
//            1, 1, 1, 1, 1, 1, 1, 1,
//            1, 0, 0, 0, 0, 0, 0, 1,
//            1, 0, 0, 0, 0, 0, 0, 1,
//            1, 0, 0, 0, 0, 0, 0, 1,
//            1, 0, 0, 0, 0, 0, 0, 1,
//            1, 0, 0, 0, 0, 0, 0, 1,
//            1, 0, 0, 0, 0, 0, 0, 1,
//            1, 1, 1, 1, 1, 1, 1, 1};


uint32_t pixels[SCREEN_WIDTH * SCREEN_WIDTH];

SDL_Window *window;
SDL_Texture *texture;
SDL_Renderer *renderer;

bool quit = false;

struct {
    float x;
    float y;
    float rot;
    float size;
} player;

struct {
    float x1;
    float y1;
    float x2;
    float y2;
} lineThing;

uint32_t setColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A) {
    uint32_t col = (16777216 * R) + (65536 * G) + (256 * B) + (1 * A);
    return col;
}

void drawMap() {

    int nextScreenY = 0;
    for (int i = 0; i < MAP_SIZE; i += 1) {     //i = y, j = x

        int screenY = nextScreenY;
        if (i + 1 < MAP_SIZE) {
            nextScreenY = ((i + 1) * SCREEN_HEIGHT) / MAP_SIZE;     //used on next iteration to set current screenY
        }
        else {
            nextScreenY = SCREEN_HEIGHT;
        }
        int gapSizeY = nextScreenY - screenY;

        int nextScreenX = 0;
        for (int j = 0; j < MAP_SIZE; j += 1) {

            int screenX = nextScreenX;
            if (j + 1 < MAP_SIZE) {
                nextScreenX = ((j + 1) * SCREEN_WIDTH) / MAP_SIZE;  //used on next iteration to set current screenX
            }
            else {
                nextScreenX = SCREEN_WIDTH;
            }
            int gapSizeX = nextScreenX - screenX;

            int wallType = map[j + (i * MAP_SIZE)];
            uint32_t screenCol;
            if (wallType == 1) {
                screenCol = 0x990044FF;
            }
            else {
                screenCol = 0x222222FF;
            }
            //gapSizeY = 10;
            //gapSizeX = 10;
            for (int ii = 0; ii < gapSizeY; ii += 1) {
                for (int jj = 0; jj < gapSizeX; jj += 1) {
                    pixels[jj + screenX + ((ii + screenY) * SCREEN_WIDTH)] = screenCol;
                }
            }
        }
    }
}

void drawMapPlayer() {

}

void drawLine(float sX, float sY, float eX, float eY, uint32_t col) {
    float startX = (sX * SCREEN_WIDTH) / MAP_SIZE;
    float startY = (sY * SCREEN_HEIGHT) / MAP_SIZE;
    float endX = (eX * SCREEN_WIDTH) / MAP_SIZE;
    float endY = (eY * SCREEN_HEIGHT) / MAP_SIZE;

    float distX = endX - startX;
    float distY = endY - startY;
    if (sX == eX && sY == eY) {
        pixels[(int)startX + ((int)startY * SCREEN_WIDTH)] = col;
        return;
    }

    if (fabs(distY) > fabs(distX)) {
        float stepX = distX / distY;
        if (distY < 0) {
            float bStartY = startY;
            startY = endY;
            endY = bStartY;

            //float bStartX = startX;
            startX = endX;
            //endX = bStartX;
        }
        float j = startX;
        for (float i = startY; i < endY; i += 1) {
            pixels[(int)j + ((int)i * SCREEN_WIDTH)] = col;
            j += stepX;
        }
    }
    else {
        float stepY = distY / distX;
        if (distX < 0) {
            float bStartX = startX;
            startX = endX;
            endX = bStartX;

            //float bStartY = startY;
            startY = endY;
            //endY = bStartY;
        }
        float j = startY;
        for (float i = startX; i < endX; i += 1) {
            pixels[(int)i + ((int)j * SCREEN_WIDTH)] = col;
            j += stepY;
        }
    }
}


int main(int argc, char const *argv[])
{
    printf("%s\n", ":)");

    if(SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL failed to initialize: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow(
        "test", 
        SDL_WINDOWPOS_CENTERED_DISPLAY(0), 
        SDL_WINDOWPOS_CENTERED_DISPLAY(0), 
        800, 
        600, //600
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


    player.x = 3;
    player.y = 3;
    player.rot = 0;
    player.size = 0.25;

    lineThing.x1 = 2;
    lineThing.y1 = 2;
    lineThing.x2 = 5;
    lineThing.y2 = 5;
    float step = 0.1;
    
    while (!quit){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }


        const uint8_t *keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_LEFT]) {
            lineThing.x1 -= step;
        }
        if (keystate[SDL_SCANCODE_RIGHT]) {
            lineThing.x1 += step;
        }
        if (keystate[SDL_SCANCODE_UP]) {
            lineThing.y1 -= step;
        }
        if (keystate[SDL_SCANCODE_DOWN]) {
            lineThing.y1 += step;
        }

        if (keystate[SDL_SCANCODE_A]) {
            lineThing.x2 -= step;
        }
        if (keystate[SDL_SCANCODE_D]) {
            lineThing.x2 += step;
        }
        if (keystate[SDL_SCANCODE_W]) {
            lineThing.y2 -= step;
        }
        if (keystate[SDL_SCANCODE_S]) {
            lineThing.y2 += step;
        }

        memset(pixels, 0, sizeof(pixels));
        drawMap();
        drawLine(lineThing.x1, lineThing.y1, lineThing.x2, lineThing.y2, 0xFFFFFFFF);

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
