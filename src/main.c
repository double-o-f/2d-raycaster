#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

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
uint32_t skyColor = 0x110033FF;
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
    double fov; //in radians rn
    double plaDist; //plane dist from player
    double plaW; //plane width

    clock_t time;
    double delta;

    double fpsCap;
} state;

struct {
    double x;
    double y;

    double maxVelo;
    double xVelo;
    double yVelo;
    double accel;
    double friction;

    double rot;
    double rSpeed;
    double pSin;
    double pCos;
    double nSin;
    double nCos;

    bool noclip;
    double ncSpeed;

    double mSpeed;
} player;


uint32_t setColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A) {
    uint32_t col = (16777216 * R) + (65536 * G) + (256 * B) + (1 * A);
    return col;
}

double getDist(double x1, double y1, double x2, double y2) {
    return sqrtf(( powf(x2 - x1, 2) ) + ( powf(y2 - y1, 2) ));
}

int getSign(double num) {
    if (num > 0) {
        return 1;
    }
    else if (num < 0) {
        return -1;
    }
    return 0;
}

void zeroOut(double* pVal, double step) {
    if (*pVal == 0) {return;}
    double val = *pVal;
    double oldVal = val;
    step *= getSign(val) * -1;
    val += step;
    if (getSign(val) != getSign(oldVal)) {
        val = 0;
    }

    *pVal = val;

}


bool collisionCheck (double x, double y) {
    if (map[(int)x + ((int)y * MAP_SIZE)] == 0) {return false;}
    return true;
}

void setPos(double x, double y) {
    player.x = x;
    player.y = y;
}

void setRot(double rot) {
    player.rot = rot;

    if (player.rot > (M_PI * 2)) {
        player.rot = 0;
    }
    else if (player.rot < 0) {
        player.rot = (M_PI * 2);
    }

    player.pSin = sin(player.rot);
    player.pCos = cos(player.rot);
    double norm = fabs(player.pSin) + fabs(player.pCos);
    player.nSin = player.pSin / norm;
    player.nCos = player.pCos / norm;

}

void playerRotate(double rSpeed) {
    setRot(player.rot += rSpeed * state.delta);
}

void mPlayerRotate(double mRot) {
    setRot(player.rot += player.mSpeed * mRot);
}

void playerNCForward(double speed) {
    player.x += cos(player.rot) * speed * state.delta;
    player.y += sin(player.rot) * speed * state.delta;
}

void playerNCbackward(double speed) {
    player.x += cos(player.rot) * speed * -1 * state.delta;
    player.y += sin(player.rot) * speed * -1 * state.delta;
}

void playerNCLeft(double speed) {
    player.x += sin(player.rot) * speed * state.delta;
    player.y += cos(player.rot) * speed * -1 * state.delta;
}

void playerNCRight(double speed) {
    player.x += sin(player.rot) * speed * -1 * state.delta;
    player.y += cos(player.rot) * speed * state.delta;
}

void playerDir(double newXVelo, double newYVelo) {
    player.xVelo = newXVelo;
    player.yVelo = newYVelo;


    double rXVelo;
    double rYVelo;

    rXVelo = player.xVelo * player.nCos;
    rYVelo = player.yVelo * player.nSin;
    if (fabs(rXVelo) + fabs(rYVelo) > player.maxVelo) {
        double vNorm = fabs(rXVelo) + fabs(rYVelo);
        player.xVelo = (player.xVelo / vNorm) * player.maxVelo;
        player.yVelo = (player.yVelo / vNorm) * player.maxVelo;
    }

    rXVelo = player.xVelo * player.nSin;
    rYVelo = player.yVelo * player.nCos;
    if (fabs(rXVelo) + fabs(rYVelo) > player.maxVelo) {
        double vNorm = fabs(rXVelo) + fabs(rYVelo);
        player.xVelo = (player.xVelo / vNorm) * player.maxVelo;
        player.yVelo = (player.yVelo / vNorm) * player.maxVelo;
    }
}

void playerForward(double accel) {
    double newXVelo = player.xVelo + player.nCos * accel * state.delta;
    double newYVelo = player.yVelo + player.nSin * accel * state.delta;

    playerDir(newXVelo, newYVelo);
}

void playerbackward(double accel) {
    double newXVelo = player.xVelo + player.nCos * accel * state.delta * -1;
    double newYVelo = player.yVelo + player.nSin * accel * state.delta * -1;

    playerDir(newXVelo, newYVelo);
}

void playerLeft(double accel) {
    double newXVelo = player.xVelo + player.nSin * accel * state.delta;
    double newYVelo = player.yVelo + player.nCos * accel * state.delta * -1;

    playerDir(newXVelo, newYVelo);
}

void playerRight(double accel) {
    double newXVelo = player.xVelo + player.nSin * accel * state.delta * -1;
    double newYVelo = player.yVelo + player.nCos * accel * state.delta;

    playerDir(newXVelo, newYVelo);
}

void playerMove() {
    bool cX =  collisionCheck(player.x + player.xVelo, player.y);
    bool cY =  collisionCheck(player.x               , player.y + player.yVelo);
    bool cXY = collisionCheck(player.x + player.xVelo, player.y + player.yVelo);

    if (!cX && !cY && !cXY) {
        player.x += player.xVelo;
        player.y += player.yVelo;
    }
    else if (!cX) {
        player.x += player.xVelo;
        if (player.y + player.yVelo < player.y) {player.y = (int)player.y;}
        else {player.y = ((int)player.y) + 0.999;}
        player.yVelo = 0;
    }
    else if (!cY) {
        player.y += player.yVelo;
        if (player.x + player.xVelo < player.x) {player.x = (int)player.x;}
        else {player.x = ((int)player.x) + 0.999;}
        player.xVelo = 0;
    }
    else {
        if (player.y + player.yVelo < player.y) {player.y = (int)player.y;}
        else {player.y = ((int)player.y) + 0.999;}
        player.yVelo = 0;
        
        if (player.x + player.xVelo < player.x) {player.x = (int)player.x;}
        else {player.x = ((int)player.x) + 0.999;}
        player.xVelo = 0;
    }

}

void playerFriction(double friction) {
    double norm = fabs(player.xVelo) + fabs(player.yVelo);

    zeroOut(&player.xVelo, friction * fabs(player.xVelo / norm) * state.delta);
    zeroOut(&player.yVelo, friction * fabs(player.yVelo / norm) * state.delta);
}


void changeFov(double fov) {
    state.fov = fov;
    state.plaW = tan(state.fov / 2) * state.plaDist * 2;
}

void playerInit(double x, double y, double rot) {
    setPos(x, y);
    player.xVelo = 0;
    player.yVelo = 0;

    setRot(rot);

    player.noclip = false;
}

void playerStats() {
    player.ncSpeed = 3;
    player.rSpeed = 3;
    player.mSpeed = 0.003;

    player.accel = 0.4;
    player.friction = 0.2;
    player.maxVelo = 0.1;
}

void stateInit() {
    state.fpsCap = (CLOCKS_PER_SEC / 60); //1000000
    state.plaDist = 1; //can be practically anything and curent renderer will still work
    state.time = clock();
    changeFov(1.745329); //1.745329 = 100, (M_PI * 2) / 4 or 1.570796 = 90
}

void changeWall() {
    int x;
    int y;
    SDL_GetMouseState(&x, &y);

    int mapX = (x * MAP_SIZE) / WINDOW_WIDTH;
    int mapY = (y * MAP_SIZE) / WINDOW_HEIGHT;

    if (map[mapX + (mapY * MAP_SIZE)] >= 3) {
        map[mapX + (mapY * MAP_SIZE)] = 0;
    }
    else {
        map[mapX + (mapY * MAP_SIZE)] += 1;
    }
}


void drawCrosshair() {
    pixels[(SCREEN_WIDTH / 2) + (SCREEN_WIDTH * (SCREEN_HEIGHT / 2))] = 0xFFFFFFFF;
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

void drawPoint(double x, double y, uint32_t col) {
    double screenX = (x * SCREEN_WIDTH) / MAP_SIZE;
    double screenY = (y * SCREEN_HEIGHT) / MAP_SIZE;

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

void castRayDDA(double rayAng, double startX, double startY, double* posX, double* posY, double* dist, int* wallType, bool* lastWasX) {
    double pSin = sin(rayAng);
    double pCos = cos(rayAng);

    double pTan = pSin / pCos;
    double pCot = pCos / pSin;

    double xStep = sqrtf(1 + pTan * pTan);
    double yStep = sqrtf(1 + pCot * pCot);
    int mapX = startX;
    int mapY = startY;

    double rayX;
    double rayY;
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

    double curDist = 0;
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

void castRayDDADraw(double rayAng, double startX, double startY, double* posX, double* posY, double* dist, int* wallType, bool* lastWasX) {
    double rSin = sin(rayAng);
    double rCos = cos(rayAng);

    double rTan = rSin / rCos;
    double rCot = rCos / rSin;

    double xStep = sqrtf(1 + rTan * rTan);
    double yStep = sqrtf(1 + rCot * rCot);
    int mapX = startX;
    int mapY = startY;

    double rayX;
    double rayY;
    int xDir;   //direction ray is going
    int yDir;

    if (rCos < 0) {
        xDir = -1;
        rayX = (startX - (int)startX) * xStep;
    }
    else {
        xDir = 1;
        rayX = (1 + (int)startX - startX) * xStep;
    }
    if (rSin < 0) {
        yDir = -1;
        rayY = (startY - (int)startY) * yStep;
    }
    else {
        yDir = 1;
        rayY = (1 + (int)startY - startY) * yStep;
    }

    double curDist = 0;
    bool last = 0;
    while (true) {
        double x = startX + (curDist / xStep) * xDir;
        double y = startY + (curDist / yStep) * yDir;

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
    double posX;
    double posY;
    double dist;
    int wallType;
    bool lastWasX;

    castRayDDADraw(player.rot, player.x, player.y, &posX, &posY, &dist, &wallType, &lastWasX);
}

void drawRaysDDA() {
    double fovOffset = (state.fov / 2) * -1; //starting fov offset
    double fovStep = (state.fov / 2) / SCREEN_WIDTH;  //move ray angle by this much every pixel

    for (int x = 0; x < SCREEN_WIDTH; x += 1) {
        double plaAng = player.rot + fovOffset + (fovStep * x);

        double posX;
        double posY;
        double dist;
        int wallType;
        bool lastWasX;

        castRayDDADraw(plaAng, player.x, player.y, &posX, &posY, &dist, &wallType, &lastWasX);

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
    double fovOffset = (state.fov / 2) * -1; //starting fov offset
    double fovStep = (state.fov / 2) / SCREEN_WIDTH;  //move ray angle by this much every pixel

    for (int x = 0; x < SCREEN_WIDTH; x += 1) {
        double plaAng = player.rot + fovOffset + (fovStep * x);

        double posX;
        double posY;
        double dist;
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
    double plaXStep = state.plaW / (SCREEN_WIDTH - 1);

    for (int x = 0; x < SCREEN_WIDTH; x += 1) {
        double plaX = x * plaXStep;
        double plaAng = atan((((state.plaW * -1) / 2) + plaX) / state.plaDist) + player.rot;

        double posX;
        double posY;
        double dist;
        int wallType;
        bool lastWasX;

        castRayDDA(plaAng, player.x, player.y, &posX, &posY, &dist, &wallType, &lastWasX);

        int wallHeight;
        double pDist = dist * cos(plaAng - player.rot);

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
        SDL_WINDOW_ALLOW_HIGHDPI); // | SDL_WINDOW_FULLSCREEN
    if(!window) {
        fprintf(stderr, "failed to create SDL window: %s\n", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC); //SDL_RENDERER_PRESENTVSYNC
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
    
    
    playerInit(4, 9, 0);
    playerStats();
    stateInit();

    bool showMap = false;
    SDL_SetRelativeMouseMode(true);
    bool fishEye = false;

    bool rJP = false;
    bool fJP = false;
    bool mbJP = false;


    while (!quit){
        
        while (clock() - state.time < state.fpsCap) {}
        //printf("%ld\n", clock() - state.time);
        state.delta = (double)(clock() - state.time) / CLOCKS_PER_SEC; //1000000
        state.time = clock();
        
        printf("%f\n", 1/state.delta);
        //printf("%ld\n", state.time - state.oldTime);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN && showMap) {
                if (!(mbJP)) {
                    changeWall();
                    mbJP = true;
                }
            }
            else {
                mbJP = false;
            }

            if (event.type == SDL_MOUSEMOTION && !showMap) {
                mPlayerRotate(event.motion.xrel);
            }
        }

        const uint8_t *keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W]) {
            playerForward(player.accel);
        }
        if (keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S]) {
            playerbackward(player.accel);
        }
        if (keystate[SDL_SCANCODE_A]) {
            playerLeft(player.accel);
        }
        if (keystate[SDL_SCANCODE_D]) {
            playerRight(player.accel);
        }

        if (keystate[SDL_SCANCODE_LEFT]) {
            playerRotate(player.rSpeed * -1);
        }
        if (keystate[SDL_SCANCODE_RIGHT]) {
            playerRotate(player.rSpeed);
        }

        if (keystate[SDL_SCANCODE_R]) {
            if (!(rJP)) {
                if (showMap) {
                    showMap = false;
                    SDL_SetRelativeMouseMode(true);
                    }
                else {
                    showMap = true;
                    SDL_SetRelativeMouseMode(false);
                    }
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

        playerMove();
        //printf("%f, %f\n", player.xVelo, player.yVelo);
        playerFriction(player.friction);

        //memset(pixels, 0, sizeof(pixels));
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
                drawCrosshair();
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
