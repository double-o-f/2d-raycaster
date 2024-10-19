#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#ifdef __WIN64
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>

#define SCREEN_WIDTH  800 // 1440// 1600
#define SCREEN_HEIGHT 450 // 1080// 900

#define WINDOW_WIDTH  1600 // 1440// 1600
#define WINDOW_HEIGHT 900 // 1080// 900

uint32_t voidColor = 0x00000000;
uint32_t color0 = 0x222222FF;
uint32_t skyColor = 0x00CCEEFF;
uint32_t floorColor = 0x222222FF;
uint32_t color1 = 0x990044FF;
uint32_t color2 = 0x004499FF;
uint32_t color3 = 0x994400FF;

//#define MAP_SIZE 6
//int map[MAP_SIZE * MAP_SIZE + 1] = {
//            0, 1, 0, 1, 0, 1,
//            1, 0, 0, 0, 0, 1,
//            1, 0, 0, 0, 0, 1,
//            1, 0, 0, 0, 0, 1,
//            1, 0, 0, 0, 0, 1,
//            1, 1, 1, 1, 1, 1};

//#define MAP_SIZE 7
//int map[MAP_SIZE * MAP_SIZE] = {
//            1, 1, 1, 1, 1, 1, 1,
//            1, 0, 2, 0, 0, 0, 1,
//            1, 0, 0, 0, 0, 2, 1,
//            1, 0, 0, 2, 0, 0, 1,
//            1, 0, 0, 0, 2, 0, 1,
//            1, 2, 0, 0, 0, 0, 1,
//            1, 1, 1, 1, 1, 1, 1};

//#define MAP_SIZE 8
//int map[MAP_SIZE * MAP_SIZE] = {  
//            1, 1, 1, 1, 1, 1, 1, 1,
//            1, 0, 0, 0, 0, 0, 0, 1,
//            1, 0, 2, 0, 2, 0, 0, 1,
//            1, 0, 0, 0, 0, 0, 0, 1,
//            1, 0, 2, 0, 2, 0, 0, 1,
//            1, 0, 0, 0, 0, 2, 0, 1,
//            1, 0, 0, 0, 0, 0, 0, 1,
//            1, 1, 1, 1, 1, 1, 1, 1};

#define MAP_SIZE 16
int map[MAP_SIZE * MAP_SIZE] = {  
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 2, 0, 1,
            1, 0, 2, 0, 3, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 2, 0, 1,
            1, 0, 2, 2, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 2, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,};


uint32_t pixels[SCREEN_WIDTH * SCREEN_WIDTH];

SDL_Window *window;
SDL_Texture *texture;
SDL_Renderer *renderer;

bool quit = false;

struct {
    float fov; //in radians rn
    float rayStepSize; //smaller means smaller ray steps (more accurate)
    float minHeight; //min height of wall
} state;

struct {
    float x;
    float y;
    float rot;
    float speed;
    float rSpeed;
} player;


uint32_t setColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A) {
    uint32_t col = (16777216 * R) + (65536 * G) + (256 * B) + (1 * A);
    return col;
}

void playerRotate(float rSpeed) {
    player.rot += rSpeed;
    if (player.rot > (M_PI * 2)) {
        player.rot = 0;
    }
    else if (player.rot < 0) {
        player.rot = (M_PI * 2);
    }
}

void playerForward(float speed) {
    player.x += cosf(player.rot) * speed;
    player.y += sinf(player.rot) * speed;
}

void playerbackward(float speed) {
    player.x += cosf(player.rot) * speed * -1;
    player.y += sinf(player.rot) * speed * -1;
}

void playerLeft(float speed) {
    player.x += sinf(player.rot) * speed;
    player.y += cosf(player.rot) * speed * -1;
}

void playerRight(float speed) {
    player.x += sinf(player.rot) * speed * -1;
    player.y += cosf(player.rot) * speed;
}

float dist(float x1, float y1, float x2, float y2) {
    return sqrtf(fabs(x2 - x1) + fabs(y2 - y1));
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
                screenCol = color1;
            }
            else if (wallType == 2) {
                screenCol = color2;
            }
            else if (wallType == 3) {
                screenCol = color3;
            }
            else {
                screenCol = color0;
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

void drawVertLine(int x, int startY, int endY, uint32_t col) {
    for (int i = startY; i < endY; i += 1) {
        pixels[x + (i * SCREEN_WIDTH)] = col;
    }
}

void fill(uint32_t col) {
    for (int i = 0; i < (SCREEN_HEIGHT * SCREEN_WIDTH); i += 1) {
        pixels[i] = col;
    }
}

void drawLine(float sX, float sY, float eX, float eY, uint32_t col) {  //draw line based on map size
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
            //if (fabs(rayX) > MAP_SIZE || fabs(rayY) > MAP_SIZE) {
            //    break;
            //}
            int rX = (int)rayX;
            int rY = (int)rayY;
            int wallType = map[(int)rayX + ((int)rayY * MAP_SIZE)];
            if (wallType != 0) {
                break;
            }

            drawLine(rayX, rayY, rayX, rayY, 0xFFFFFFFF);
            rayX += pCos * state.rayStepSize;
            rayY += pSin * state.rayStepSize;
        }
        fovOffset += fovStep;
    }
}

void drawWolf() {
    float fovOffset = (state.fov / 2) * -1; //starting fov offset
    float fovStep = (state.fov / 2) / SCREEN_WIDTH;  //move ray angle by this much every pixel

    for (int i = 0; i < SCREEN_WIDTH; i += 1) {
        float startX = player.x; //starting point of ray
        float startY = player.y; //^^
        float rayX = startX;
        float rayY = startY;
        float pSin = sinf(player.rot + fovOffset + (fovStep * i));
        float pCos = cosf(player.rot + fovOffset + (fovStep * i));

        while(true) {
            int wallType = map[(int)rayX + ((int)rayY * MAP_SIZE)];
            if (wallType == 1) {
                //int wallHeight = ((maxDist - dist(startX, startY, rayX, rayY)) * SCREEN_HEIGHT) / MAP_SIZE;
                int wallHeight;
                int rayDist = dist(startX, startY, rayX, rayY);
                if (rayDist < 1) {
                    wallHeight = SCREEN_HEIGHT;
                }
                else {
                    wallHeight = SCREEN_HEIGHT / dist(startX, startY, rayX, rayY);
                }
                int wallStart = (SCREEN_HEIGHT - wallHeight) / 2;
                drawVertLine(i, 0, wallStart, skyColor);
                drawVertLine(i, wallStart, (wallStart + wallHeight), color1);
                drawVertLine(i, (wallStart + wallHeight), SCREEN_HEIGHT, floorColor);
                break;
            }
            if (wallType == 2) {
                int wallHeight;
                int rayDist = dist(startX, startY, rayX, rayY);
                if (rayDist < 1) {
                    wallHeight = SCREEN_HEIGHT;
                }
                else {
                    wallHeight = SCREEN_HEIGHT / dist(startX, startY, rayX, rayY);
                }
                int wallStart = (SCREEN_HEIGHT - wallHeight) / 2;
                drawVertLine(i, 0, wallStart, skyColor);
                drawVertLine(i, wallStart, (wallStart + wallHeight), color2);
                drawVertLine(i, (wallStart + wallHeight), SCREEN_HEIGHT, floorColor);
                break;
            }
            if (wallType == 3) {
                int wallHeight;
                int rayDist = dist(startX, startY, rayX, rayY);
                if (rayDist < 1) {
                    wallHeight = SCREEN_HEIGHT;
                }
                else {
                    wallHeight = SCREEN_HEIGHT / dist(startX, startY, rayX, rayY);
                }
                int wallStart = (SCREEN_HEIGHT - wallHeight) / 2;
                drawVertLine(i, 0, wallStart, skyColor);
                drawVertLine(i, wallStart, (wallStart + wallHeight), color3);
                drawVertLine(i, (wallStart + wallHeight), SCREEN_HEIGHT, floorColor);
                break;
            }

            rayX += pCos * state.rayStepSize;
            rayY += pSin * state.rayStepSize;
        }
        fovOffset += fovStep;
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
        WINDOW_WIDTH, 
        WINDOW_HEIGHT, //600
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
    player.speed = 0.05;
    player.rSpeed = 0.05;
    
    state.rayStepSize = 0.01;
    state.fov = ((M_PI * 2) / 4); //90
    bool showMap = false;
    bool rJP = false;

    while (!quit){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        const uint8_t *keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_LEFT]) {
            playerRotate(player.rSpeed * -1);
        }
        if (keystate[SDL_SCANCODE_RIGHT]) {
            playerRotate(player.rSpeed);
        }
        if (keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W]) {
            playerForward(player.speed);
        }
        if (keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S]) {
            playerbackward(player.speed);
        }

        if (keystate[SDL_SCANCODE_A]) {
            playerLeft(player.speed);
        }
        if (keystate[SDL_SCANCODE_D]) {
            playerRight(player.speed);
        }

        if (keystate[SDL_SCANCODE_R]) {
            if (!(rJP)) {
                if (showMap) {showMap = false;}
                else {showMap = true;}
                rJP = true;
            }
        }
        else {
            rJP = false;
        }


        memset(pixels, 0, sizeof(pixels));
        if (showMap) {
            memset(pixels, 0, sizeof(pixels));
            drawMap();
            drawLine(player.x, player.y, player.x, player.y, 0xFFFFFFFF);
            drawRays();
        }
        else {
            //fill(color0);
            drawWolf();
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
