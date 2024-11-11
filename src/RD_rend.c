#include <stdlib.h> //malloc
#include <stdbool.h> //bool
#include <math.h> //sin, cos, tan, atan, sqrtf

#include "PL_player.h" //PL_player.(x, y, rot)
#include "MP_map.h" //MP_map.(map, width, height)
#include "UI_ui.h" //MP_map.showMap

#include "RD_rend.h"
struct RD_rend_ RD_rend;


void RD_drawCrosshair() {
    RD_rend.pixels[(RD_rend.screenWidth / 2) + (RD_rend.screenWidth * (RD_rend.screenHeight / 2))] = 0xFFFFFFFF;
}

void RD_drawPoint(double x, double y, uint32_t col) {
    double screenX = (x * RD_rend.screenWidth) / MP_map.width;
    double screenY = (y * RD_rend.screenHeight) / MP_map.height;

    RD_rend.pixels[(int)screenX + ((int)screenY * RD_rend.screenWidth)] = col;
}

void RD_fillScreen(uint32_t col) {
    for (int i = 0; i < (RD_rend.screenWidth * RD_rend.screenHeight); i += 1) {
        RD_rend.pixels[i] = col;
    }
}

void RD_drawVertLine(int x, int startY, int endY, uint32_t col) {
    for (int y = startY; y < endY; y += 1) {
        RD_rend.pixels[x + (y * RD_rend.screenWidth)] = col;
    }
}

void RD_drawWallSlice(int x, int wallHeight, int wallType, uint32_t bright) {
    uint32_t col;
    if (wallType == 1)
    {
        col = RD_rend.color1;
    }
    else if (wallType == 2)
    {
        col = RD_rend.color2;
    }
    else if (wallType == 3)
    {
        col = RD_rend.color3;
    }
    else {
        col = (uint32_t)wallType;   //this is bad but it looks cool
    }

    col &= bright;
    int wallStart = (RD_rend.screenHeight - wallHeight) / 2;
    RD_drawVertLine(x, 0, wallStart, RD_rend.skyColor);
    RD_drawVertLine(x, wallStart, (wallStart + wallHeight), col);
    RD_drawVertLine(x, (wallStart + wallHeight), RD_rend.screenHeight, RD_rend.floorColor);
}


void RD_castRay(double rayAng, double startX, double startY, double* posX, double* posY, double* dist, int* wallType, bool* lastWasX) {
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
        int wall = MP_map.map[mapX + (mapY * MP_map.width)];
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

void RD_castRayDraw(double rayAng, double startX, double startY, double* posX, double* posY, double* dist, int* wallType, bool* lastWasX) {
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

        RD_drawPoint(x, y, 0xFFFFFFFF);
        int wall = MP_map.map[mapX + (mapY * MP_map.width)];
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


void RD_drawRay() {
    double posX;
    double posY;
    double dist;
    int wallType;
    bool lastWasX;

    RD_castRayDraw(PL_player.rot, PL_player.x, PL_player.y, &posX, &posY, &dist, &wallType, &lastWasX);
}

void RD_drawRays() {
    double fovOffset = (RD_rend.fov / 2) * -1; //starting fov offset
    double fovStep = (RD_rend.fov / 2) / RD_rend.screenWidth;  //move ray angle by this much every pixel

    for (int x = 0; x < RD_rend.screenWidth; x += 1) {
        double plaAng = PL_player.rot + fovOffset + (fovStep * x);

        double posX;
        double posY;
        double dist;
        int wallType;
        bool lastWasX;

        RD_castRayDraw(plaAng, PL_player.x, PL_player.y, &posX, &posY, &dist, &wallType, &lastWasX);

        if (x == 0 || x == RD_rend.screenWidth - 1) {
            RD_castRayDraw(plaAng, PL_player.x, PL_player.y, &posX, &posY, &dist, &wallType, &lastWasX);
        }
        else {
            RD_castRay(plaAng, PL_player.x, PL_player.y, &posX, &posY, &dist, &wallType, &lastWasX);
        }
        RD_drawPoint(posX, posY, 0xFFFFFFFF);

        fovOffset += fovStep;
    }
}


void RD_drawMap() {
    int nextScreenY = 0;
    for (int y = 0; y < MP_map.height; y += 1) {

        int screenY = nextScreenY;
        if (y + 1 < MP_map.height) {
            nextScreenY = ((y + 1) * RD_rend.screenHeight) / MP_map.height;     //used on next iteration to set current screenY
        }
        else {
            nextScreenY = RD_rend.screenHeight;
        }
        int gapSizeY = nextScreenY - screenY;

        int nextScreenX = 0;
        for (int x = 0; x < MP_map.width; x += 1) {

            int screenX = nextScreenX;
            if (x + 1 < MP_map.width) {
                nextScreenX = ((x + 1) * RD_rend.screenWidth) / MP_map.width;  //used on next iteration to set current screenX
            }
            else {
                nextScreenX = RD_rend.screenWidth;
            }
            int gapSizeX = nextScreenX - screenX;

            int wallType = MP_map.map[x + (y * MP_map.width)];
            uint32_t screenCol;
            if (wallType == 1) {
                screenCol = RD_rend.color1;
            }
            else if (wallType == 2) {
                screenCol = RD_rend.color2;
            }
            else if (wallType == 3) {
                screenCol = RD_rend.color3;
            }
            else {
                screenCol = RD_rend.color0;
            }

            for (int i = 0; i < gapSizeY; i += 1) {     //i = y, j = x
                for (int j = 0; j < gapSizeX; j += 1) {
                    RD_rend.pixels[j + screenX + ((i + screenY) * RD_rend.screenWidth)] = screenCol;
                }
            }
        }
    }
}

void RD_drawFish() {
    double fovOffset = (RD_rend.fov / 2) * -1; //starting fov offset
    double fovStep = (RD_rend.fov / 2) / RD_rend.screenWidth;  //move ray angle by this much every pixel

    for (int x = 0; x < RD_rend.screenWidth; x += 1) {
        double plaAng = PL_player.rot + fovOffset + (fovStep * x);

        double posX;
        double posY;
        double dist;
        int wallType;
        bool lastWasX;

        RD_castRay(plaAng, PL_player.x, PL_player.y, &posX, &posY, &dist, &wallType, &lastWasX);

        int wallHeight;
        if (dist < 1) {
            wallHeight = RD_rend.screenHeight;
        }
        else {
            wallHeight = RD_rend.screenHeight / dist;
        }

        if (lastWasX) {
            RD_drawWallSlice(x, wallHeight, wallType, 0xC0C0C0FF);
        }
        else {
            RD_drawWallSlice(x, wallHeight, wallType, 0xFFFFFFFF);
        }

        fovOffset += fovStep;
    }
}

void RD_drawWolf() {
    double plaXStep = RD_rend.plaW / (RD_rend.screenWidth - 1);

    for (int x = 0; x < RD_rend.screenWidth; x += 1) {
        double plaX = x * plaXStep;
        double plaAng = atan((((RD_rend.plaW * -1) / 2) + plaX) / RD_rend.plaDist) + PL_player.rot;

        double posX;
        double posY;
        double dist;
        int wallType;
        bool lastWasX;

        RD_castRay(plaAng, PL_player.x, PL_player.y, &posX, &posY, &dist, &wallType, &lastWasX);

        int wallHeight;
        double pDist = dist * cos(plaAng - PL_player.rot);

        if (pDist < 1) {
            wallHeight = RD_rend.screenHeight;
        }
        else {
            wallHeight = RD_rend.screenHeight / pDist;
        }

        if (lastWasX) {
            RD_drawWallSlice(x, wallHeight, wallType, 0xC0C0C0FF);
        }
        else {
            RD_drawWallSlice(x, wallHeight, wallType, 0xFFFFFFFF);
        }
    }
}


void RD_drawStuff() {
    if (UI_ui.showMap) {
        RD_drawMap();
        RD_drawPoint(PL_player.x, PL_player.y, 0xFFFFFFFF);
        RD_drawRays();
    }
    else if (UI_ui.showGame) {
        if (RD_rend.fishEye) {
            RD_drawFish();
        }
        else {
            RD_drawWolf();
        }
        RD_drawCrosshair();
    }
    else {
        RD_fillScreen(0x30003000);
    }
}


void RD_changeFov(double fov) {
    RD_rend.fov = fov;
    RD_rend.plaW = tan(RD_rend.fov / 2) * RD_rend.plaDist * 2;
}

void RD_init() {
    RD_rend.screenWidth = 800; // 1440// 1600// 800
    RD_rend.screenHeight = 450; // 1080// 900// 450

    RD_rend.pixels = (uint32_t*)malloc(RD_rend.screenWidth * RD_rend.screenHeight * sizeof(uint32_t));

    RD_rend.plaDist = 1; //can be practically anything and curent renderer will still work
    RD_changeFov(1.745329); //1.745329 = 100, (M_PI * 2) / 4 or 1.570796 = 90

    RD_rend.fishEye = false;

    RD_rend.voidColor = 0x000000FF;
    RD_rend.color0 = 0x222222FF;
    RD_rend.skyColor = 0x110033FF;
    RD_rend.floorColor = 0x222222FF;
    RD_rend.color1 = 0x990044FF;
    RD_rend.color2 = 0x004499FF;
    RD_rend.color3 = 0x994400FF;
}
