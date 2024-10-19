#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#ifdef __WIN64
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>

#define SCREEN_WIDTH  400 // 1440// 
#define SCREEN_HEIGHT 400 // 1080// 60

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
            1, 0, 2, 0, 2, 0, 1,
            1, 0, 0, 0, 0, 0, 1,
            1, 0, 2, 0, 2, 0, 1,
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
    float fov; //in radians rn
    float rayStepSize; //smaller means smaller ray steps (more accurate)
} state;

struct {
    float x;
    float y;
    float rot;
    float size;
    float speed;
    float rSpeed;
} player;


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
            else if (wallType == 2) {
                screenCol = 0x004499FF;
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

    if (sX == eX && sY == eY) {
        pixels[(int)startX + ((int)startY * SCREEN_WIDTH)] = col;
        return;
    }

    float distX = endX - startX;
    float distY = endY - startY;


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

void drawRays() {
    float fovOffset = (state.fov / 2) * -1; //starting fov offset
    float fovStep = (state.fov / 2) / SCREEN_WIDTH;  //move ray angle by this much every pixel

    for (int i = 0; i < SCREEN_WIDTH; i += 1) {
        float rayX = player.x; //starting point of ray
        float rayY = player.y; //^^
        float pSin = sinf(player.rot + fovOffset + (fovStep * i));
        float pCos = cosf(player.rot + fovOffset + (fovStep * i));

        while(true) {
            if (fabs(rayX) > MAP_SIZE || fabs(rayY) > MAP_SIZE) {
                //pixels[i] = 0x00000000;
                break;
            }
            int rX = (int)rayX;
            int rY = (int)rayY;
            int wallType = map[(int)rayX + ((int)rayY * MAP_SIZE)];
            if (wallType == 1) {
                //pixels[i] = 0x990044FF;
                break;
            }
            if (wallType == 2) {
                //pixels[i] = 0x004499FF;
                break;
            }

            drawLine(rayX, rayY, rayX, rayY, 0xFFFFFFFF);
            rayX += pCos * state.rayStepSize;
            rayY += pSin * state.rayStepSize;
        }
        fovOffset += fovStep;
    }
}

void rotatePlayer(float rSpeed) {
    player.rot += rSpeed;
    if (player.rot > (M_PI * 2)) {
        player.rot = 0;
    }
    else if (player.rot < 0) {
        player.rot = (M_PI * 2);
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
        800, //600
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
    player.speed = 0.05;
    player.rSpeed = 0.05;
    
    state.rayStepSize = 0.25;
    state.fov = (M_PI / 2); //90
    
    while (!quit){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        const uint8_t *keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_LEFT]) {
            rotatePlayer(player.rSpeed * -1);
        }
        if (keystate[SDL_SCANCODE_RIGHT]) {
            rotatePlayer(player.rSpeed);
        }
        if (keystate[SDL_SCANCODE_UP]) {
            player.y -= player.speed;
        }
        if (keystate[SDL_SCANCODE_DOWN]) {
            player.y += player.speed;
        }

        if (keystate[SDL_SCANCODE_A]) {
            player.x -= player.speed;
        }
        if (keystate[SDL_SCANCODE_D]) {
            player.x += player.speed;
        }
        if (keystate[SDL_SCANCODE_W]) {
            player.y -= player.speed;
        }
        if (keystate[SDL_SCANCODE_S]) {
            player.y += player.speed;
        }

        memset(pixels, 0, sizeof(pixels));
        drawMap();
        //drawLine(player.x, player.y, player.x, player.y, 0xFFFFFFFF);
        drawRays();

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
