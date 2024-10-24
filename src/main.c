#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#ifdef __WIN64
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>

#define SCREEN_WIDTH  1600 // 1440// 1600// 800
#define SCREEN_HEIGHT 900 // 1080// 900// 450

#define WINDOW_WIDTH  1920 // 1440// 1600// 800
#define WINDOW_HEIGHT 1080 // 1080// 900// 450

uint32_t voidColor = 0x000000FF;
uint32_t color0 = 0x222222FF;
uint32_t skyColor = 0x00CCEEFF;
uint32_t floorColor = 0x222222FF;
uint32_t color1 = 0x990044FF;
uint32_t color2 = 0x004499FF;
uint32_t color3 = 0x994400FF;


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
    float plaDist; //plane dist from player
    float rayStepSize; //smaller means smaller ray steps (more accurate)
    float plaW; //plane width
} state;

struct {
    float x;
    float y;
    float rot;
    float speed;
    float rSpeed;
    bool noclip;
} player;


uint32_t setColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A) {
    uint32_t col = (16777216 * R) + (65536 * G) + (256 * B) + (1 * A);
    return col;
}

float getDist(float x1, float y1, float x2, float y2) {
    return sqrtf(( powf(x2 - x1, 2) ) + ( powf(y2 - y1, 2) ));
}

void calcPlaW() {
    state.plaW = tanf(state.fov / 2) * state.plaDist * 2;
}

void changeFov(float fov) {
    state.fov = fov;
    calcPlaW();
}

bool collisionCheck (float x, float y) {
    if (map[(int)x + ((int)y * MAP_SIZE)] == 0) {return false;}
    return true;
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
    float xStep = cosf(player.rot) * speed;
    float yStep = sinf(player.rot) * speed;

    if (!collisionCheck(player.x + xStep, player.y + yStep) || player.noclip) {
        player.x += xStep;
        player.y += yStep;
    }
    else {
        if (!collisionCheck(player.x + xStep, player.y) || player.noclip) {
            player.x += xStep;
        }
        if (!collisionCheck(player.x, player.y + yStep) || player.noclip) {
            player.y += yStep;
        }
    }
}

void playerbackward(float speed) {
    float xStep = cosf(player.rot) * speed * -1;
    float yStep = sinf(player.rot) * speed * -1;

    if (!collisionCheck(player.x + xStep, player.y + yStep) || player.noclip) {
        player.x += xStep;
        player.y += yStep;
    }
    else {
        if (!collisionCheck(player.x + xStep, player.y) || player.noclip) {
            player.x += xStep;
        }
        if (!collisionCheck(player.x, player.y + yStep) || player.noclip) {
            player.y += yStep;
        }
    }
}

void playerLeft(float speed) {
    float xStep = sinf(player.rot) * speed;
    float yStep = cosf(player.rot) * speed * -1;

    if (!collisionCheck(player.x + xStep, player.y + yStep) || player.noclip) {
        player.x += xStep;
        player.y += yStep;
    }
    else {
        if (!collisionCheck(player.x + xStep, player.y) || player.noclip) {
            player.x += xStep;
        }
        if (!collisionCheck(player.x, player.y + yStep) || player.noclip) {
            player.y += yStep;
        }
    }
}

void playerRight(float speed) {
    float xStep = sinf(player.rot) * speed * -1;
    float yStep = cosf(player.rot) * speed;

    if (!collisionCheck(player.x + xStep, player.y + yStep) || player.noclip) {
        player.x += xStep;
        player.y += yStep;
    }
    else {
        if (!collisionCheck(player.x + xStep, player.y) || player.noclip) {
            player.x += xStep;
        }
        if (!collisionCheck(player.x, player.y + yStep) || player.noclip) {
            player.y += yStep;
        }
    }
}

void fillScreen(uint32_t col) {
    for (int i = 0; i < (SCREEN_HEIGHT * SCREEN_WIDTH); i += 1) {
        pixels[i] = col;
    }
}

void drawVertLine(int x, int startY, int endY, uint32_t col) {
    for (int y = startY; y < endY; y += 1) {
        pixels[x + (y * SCREEN_WIDTH)] = col;
    }
}

void drawPoint(float x, float y, uint32_t col) {
    float screenX = (x * SCREEN_WIDTH) / MAP_SIZE;
    float screenY = (y * SCREEN_HEIGHT) / MAP_SIZE;

    pixels[(int)screenX + ((int)screenY * SCREEN_WIDTH)] = col;
}

void drawMap() {
    int nextScreenY = 0;
    for (int y = 0; y < MAP_SIZE; y += 1) {

        int screenY = nextScreenY;
        if (y + 1 < MAP_SIZE) {
            nextScreenY = ((y + 1) * SCREEN_HEIGHT) / MAP_SIZE;     //used on next iteration to set current screenY
        }
        else {
            nextScreenY = SCREEN_HEIGHT;
        }
        int gapSizeY = nextScreenY - screenY;

        int nextScreenX = 0;
        for (int x = 0; x < MAP_SIZE; x += 1) {

            int screenX = nextScreenX;
            if (x + 1 < MAP_SIZE) {
                nextScreenX = ((x + 1) * SCREEN_WIDTH) / MAP_SIZE;  //used on next iteration to set current screenX
            }
            else {
                nextScreenX = SCREEN_WIDTH;
            }
            int gapSizeX = nextScreenX - screenX;

            int wallType = map[x + (y * MAP_SIZE)];
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

            for (int i = 0; i < gapSizeY; i += 1) {     //i = y, j = x
                for (int j = 0; j < gapSizeX; j += 1) {
                    pixels[j + screenX + ((i + screenY) * SCREEN_WIDTH)] = screenCol;
                }
            }
        }
    }
}

void drawWallSlice(int x, int wallHeight, int wallType, uint32_t bright) {
    uint32_t col;
    if (wallType == 1)
    {
        col = color1;
    }
    else if (wallType == 2)
    {
        col = color2;
    }
    else if (wallType == 3)
    {
        col = color3;
    }
    else {
        col = (uint32_t)wallType;   //this is bad but it looks cool
    }

    col &= bright;
    int wallStart = (SCREEN_HEIGHT - wallHeight) / 2;
    drawVertLine(x, 0, wallStart, skyColor);
    drawVertLine(x, wallStart, (wallStart + wallHeight), col);
    drawVertLine(x, (wallStart + wallHeight), SCREEN_HEIGHT, floorColor);
}

void castRayDDA(float rayAng, float startX, float startY, float* posX, float* posY, float* dist, int* wallType, bool* lastWasX) {
    float pSin = sinf(rayAng);
    float pCos = cosf(rayAng);

    float pTan = pSin / pCos;
    float pCot = pCos / pSin;

    float xStep = sqrtf(1 + pTan * pTan);
    float yStep = sqrtf(1 + pCot * pCot);
    int mapX = startX;
    int mapY = startY;

    float rayX;
    float rayY;
    int xDir;   //direction ray is going
    int yDir;

    if (pCos < 0) {
        xDir = -1;
        rayX = (startX - (int)startX) * xStep;
    }
    else {
        xDir = 1;
        rayX = (1 + (int)startX - startX) * xStep;
    }
    if (pSin < 0) {
        yDir = -1;
        rayY = (startY - (int)startY) * yStep;
    }
    else {
        yDir = 1;
        rayY = (1 + (int)startY - startY) * yStep;
    }

    float curDist = 0;
    bool last = 0;
    while (true) {


        int wall = map[mapX + (mapY * MAP_SIZE)];
        if (wall != 0) {
            *posX = startX + (curDist / xStep) * xDir;
            *posY = startY + (curDist / yStep) * yDir;
            *dist = curDist;
            *wallType = wall;
            *lastWasX = last;
            return;
        }

        if (rayX < rayY) {
            curDist = rayX;
            rayX += xStep;
            mapX += xDir;
            last = 1;
        }
        else {
            curDist = rayY;
            rayY += yStep;
            mapY += yDir;
            last = 0;
        }
    }
}

void castRayDDADraw(float rayAng, float startX, float startY, float* posX, float* posY, float* dist, int* wallType, bool* lastWasX) {
    float pSin = sinf(rayAng);
    float pCos = cosf(rayAng);

    float pTan = pSin / pCos;
    float pCot = pCos / pSin;

    float xStep = sqrtf(1 + pTan * pTan);
    float yStep = sqrtf(1 + pCot * pCot);
    int mapX = startX;
    int mapY = startY;

    float rayX;
    float rayY;
    int xDir;   //direction ray is going
    int yDir;

    if (pCos < 0) {
        xDir = -1;
        rayX = (startX - (int)startX) * xStep;
    }
    else {
        xDir = 1;
        rayX = (1 + (int)startX - startX) * xStep;
    }
    if (pSin < 0) {
        yDir = -1;
        rayY = (startY - (int)startY) * yStep;
    }
    else {
        yDir = 1;
        rayY = (1 + (int)startY - startY) * yStep;
    }

    float curDist = 0;
    bool last = 0;
    while (true) {
        float x = startX + (curDist / xStep) * xDir;
        float y = startY + (curDist / yStep) * yDir;

        drawPoint(x, y, 0xFFFFFFFF);
        int wall = map[mapX + (mapY * MAP_SIZE)];
        if (wall != 0) {
            *posX = x;
            *posY = y;
            *dist = curDist;
            *wallType = wall;
            *lastWasX = last;
            return;
        }

        if (rayX < rayY) {
            curDist = rayX;
            rayX += xStep;
            mapX += xDir;
            last = 1;
        }
        else {
            curDist = rayY;
            rayY += yStep;
            mapY += yDir;
            last = 0;
        }
    }
}

void drawRayDDA() {
    float posX;
    float posY;
    float dist;
    int wallType;
    bool lastWasX;

    castRayDDADraw(player.rot, player.x, player.y, &posX, &posY, &dist, &wallType, &lastWasX);
}

void drawRaysDDA() {

    float fovOffset = (state.fov / 2) * -1; //starting fov offset
    float fovStep = (state.fov / 2) / SCREEN_WIDTH;  //move ray angle by this much every pixel

    for (int x = 0; x < SCREEN_WIDTH; x += 1) {
        float plaAng = player.rot + fovOffset + (fovStep * x);

        float posX;
        float posY;
        float dist;
        int wallType;
        bool lastWasX;

        if (x == 0 || x == SCREEN_WIDTH - 1) {
            castRayDDADraw(plaAng, player.x, player.y, &posX, &posY, &dist, &wallType, &lastWasX);
        }
        else {
            castRayDDA(plaAng, player.x, player.y, &posX, &posY, &dist, &wallType, &lastWasX);
        }
        drawPoint(posX, posY, 0xFFFFFFFF);

        fovOffset += fovStep;
    }
}

void drawFishDDA() {
    float fovOffset = (state.fov / 2) * -1; //starting fov offset
    float fovStep = (state.fov / 2) / SCREEN_WIDTH;  //move ray angle by this much every pixel

    for (int x = 0; x < SCREEN_WIDTH; x += 1) {
        float plaAng = player.rot + fovOffset + (fovStep * x);

        float posX;
        float posY;
        float dist;
        int wallType;
        bool lastWasX;

        castRayDDA(plaAng, player.x, player.y, &posX, &posY, &dist, &wallType, &lastWasX);

        int wallHeight;
        if (dist < 1) {
            wallHeight = SCREEN_HEIGHT;
        }
        else {
            wallHeight = SCREEN_HEIGHT / dist;
        }

        if (lastWasX) {
            drawWallSlice(x, wallHeight, wallType, 0xC0C0C0FF);
        }
        else {
            drawWallSlice(x, wallHeight, wallType, 0xFFFFFFFF);
        }

        fovOffset += fovStep;
    }
}

void drawWolfDDA() {
    float plaXStep = state.plaW / (SCREEN_WIDTH - 1);

    for (int x = 0; x < SCREEN_WIDTH; x += 1) {
        float plaX = x * plaXStep;
        float plaAng = atanf((((state.plaW * -1) / 2) + plaX) / state.plaDist) + player.rot;

        float posX;
        float posY;
        float dist;
        int wallType;
        bool lastWasX;

        castRayDDA(plaAng, player.x, player.y, &posX, &posY, &dist, &wallType, &lastWasX);

        int wallHeight;
        float pDist = dist * cosf(plaAng - player.rot);

        if (pDist < 1) {
            wallHeight = SCREEN_HEIGHT;
        }
        else {
            wallHeight = SCREEN_HEIGHT / pDist;
        }

        if (lastWasX) {
            drawWallSlice(x, wallHeight, wallType, 0xC0C0C0FF);
        }
        else {
            drawWallSlice(x, wallHeight, wallType, 0xFFFFFFFF);
        }
    }
}


int main(int argc, char const *argv[]) {
    printf("%s\n", ":)");

    if(SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL failed to initialize: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow(
        "ray", 
        SDL_WINDOWPOS_CENTERED_DISPLAY(0), 
        SDL_WINDOWPOS_CENTERED_DISPLAY(0), 
        WINDOW_WIDTH, 
        WINDOW_HEIGHT,
        SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN);
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


    player.x = 4;
    player.y = 9;
    player.rot = 0;
    player.speed = 0.05;
    player.rSpeed = 0.05;
    player.noclip = true;
    
    state.rayStepSize = 0.01;
    state.plaDist = 1;
    changeFov((M_PI * 2) / 4);

    bool showMap = false;
    bool fishEye = false;
    bool rJP = false;
    bool fJP = false;

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
        if (keystate[SDL_SCANCODE_F]) {
            if (!(fJP)) {
                if (fishEye) {fishEye = false;}
                else {fishEye = true;}
                fJP = true;
            }
        }
        else {
            fJP = false;
        }


        // /memset(pixels, 0, sizeof(pixels));
        if (showMap) {
            drawMap();
            drawPoint(player.x, player.y, 0xFFFFFFFF);
            drawRaysDDA();
        }
        else {
            if (fishEye) {
                drawFishDDA();
            }
            else {
                drawWolfDDA();
            }
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
