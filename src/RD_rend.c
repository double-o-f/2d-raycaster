#include <stdlib.h> //malloc
#include <stdbool.h> //bool
#include <math.h> //sin, cos, tan, atan, sqrtf

#include <stdio.h>

#include "PL_player.h" //PL_player.(x, y, rot)
#include "MP_map.h" //MP_map.(map, width, height)
#include "UI_ui.h" //MP_map.showMap

#include "RD_rend.h"
struct RD_rend_ RD_rend;

struct {
    uint32_t voidColor;
    uint32_t skyColor;
    uint32_t floorColor;
    uint32_t color0;
    uint32_t color1;
    uint32_t color2;
    uint32_t color3;
} RD_colors;

struct {
    uint32_t textures[3][16 * 16];
} RD_textures;

struct RD_ray_ {
    RD_ray* lastRay;

    //double posX;
    //double posY;
    double dist;
    double texPosX;
    int wallType;
    bool lastWasX;
};
typedef struct RD_ray_ RD_ray;


void RD_drawCrosshair() {
    RD_rend.pixels[(RD_rend.screenWidth / 2) + (RD_rend.screenWidth * (RD_rend.screenHeight / 2))] = 0xFFFFFFFF;
}

void RD_drawPoint(double x, double y, uint32_t col) {
    double screenX = (x * RD_rend.screenWidth) / MP_map.width;
    double screenY = (y * RD_rend.screenHeight) / MP_map.height;

    RD_rend.pixels[(int)screenX + ((int)screenY * RD_rend.screenWidth)] = col;
}

//void RD_blackScreen() {
//    memset(RD_rend.pixels, 0, RD_rend.screenWidth * RD_rend.screenHeight * sizeof(uint32_t));
//}

void RD_fillScreen(uint32_t col) {
    for (int i = 0; i < (RD_rend.screenWidth * RD_rend.screenHeight); i += 1) {
        RD_rend.pixels[i] = col;
    }
}

void RD_drawVertLine(int x, int startY, int endY, uint32_t col) {
    
    if ((uint8_t)col == 0) {return;}
    
    for (int y = startY; y < endY; y += 1) { 
        RD_rend.pixels[x + (y * RD_rend.screenWidth)] = col;
    }
}

void RD_drawFloor(int x) {
    int mid = RD_rend.screenHeight / 2;
    RD_drawVertLine(x, 0, mid, RD_colors.skyColor);
    RD_drawVertLine(x, mid, RD_rend.screenHeight, RD_colors.floorColor);
}

void RD_drawWallSliceUpper(int x, double plaAng, bool fish, RD_ray* ray) {
    double pDist = (*ray).dist;
    if (!fish) {
        pDist *= cos(plaAng - PL_player.rot);
    }

    if (pDist < 1) { //if pDist < 1 too close to see upper wall so return
        free(ray);
        return;
    }
    
    int wallHeight = RD_rend.screenHeight / pDist;
    int wallEnd = (RD_rend.screenHeight - wallHeight) / 2;
    int wallStart = wallEnd - wallHeight;
    double texPosY = 0;
    double texStep = pDist / RD_rend.screenHeight; //how much texPosY is stepped by


    if (wallStart < 0) {
        texPosY = fabs(wallStart) * texStep;
        wallStart = 0;
    }

    uint32_t texNum = 0;
    if ((*ray).wallType == 3) {
        texNum = 3;
    }
    else if ((*ray).wallType == 100) {
        wallStart = 0;
        wallEnd = 0;
    }

    uint32_t shade;
    if ((*ray).lastWasX) {
        shade = 0xC0C0C0FF;
    }
    else {
        shade = 0xFFFFFFFF;
    }
    

    for (int y = wallStart; y < wallEnd; y += 1) {
        int texIndex = (int)(texPosY * 16) + ((int)( (*ray).texPosX * 16) * 16); //texture is flipped and rotated for caching
        uint32_t col = RD_textures.textures[texNum - 1][texIndex];
            
        if ((uint8_t)col != 0) {
            RD_rend.pixels[x + (y * RD_rend.screenWidth)] = (col & shade);
        }
        texPosY += texStep;
    }

    free(ray);
    
    //RD_drawVertLine(x, 0, wallStart, RD_rend.skyColor);
    //RD_drawVertLine(x, wallStart, wallEnd, col);
}

void RD_drawWallSliceLower(int x, double plaAng, bool fish, RD_ray* ray) {
    while (ray != NULL) {

    double pDist = (*ray).dist;
    if (!fish) {
        pDist *= cos(plaAng - PL_player.rot);
    }
    
    int wallHeight = RD_rend.screenHeight / pDist;
    int wallStart = (RD_rend.screenHeight - wallHeight) / 2;
    int wallEnd;
    double texPosY;
    double texStep = pDist / RD_rend.screenHeight; //how much texPosY is stepped by. = 1 / (RD_rend.screenHeight / pDist)
    
    if (pDist < 1) {
        wallEnd = RD_rend.screenHeight;
        texPosY = fabs(wallStart) * texStep;
        wallStart = 0;
    } 
    else {
        wallEnd = wallStart + wallHeight;
        texPosY = 0;
    }
    
    int texNum = 0;
    if ((*ray).wallType == 3)
    {
        texNum = 3;
    }
    else if ((*ray).wallType == 17) {
        texNum = 1;
    }
    else if ((*ray).wallType == 34)
    {
        texNum = 2;
    }
    else if ((*ray).wallType == 100) {
        wallStart = 0;
        wallEnd = 0;
    }

    uint32_t shade;
    if ((*ray).lastWasX) {
        shade = 0xC0C0C0FF;
    }
    else {
        shade = 0xFFFFFFFF;
    }

    
    for (int y = wallStart; y < wallEnd; y += 1) {
        
        int texIndex = (int)(texPosY * 16) + ((int)( (*ray).texPosX * 16) * 16); //texture is flipped and rotated for caching
        uint32_t col = RD_textures.textures[texNum - 1][texIndex];
            
        if ((uint8_t)col != 0) {
            RD_rend.pixels[x + (y * RD_rend.screenWidth)] = (col & shade);
        }
        
        texPosY += texStep;
    }

    RD_ray* oldRay = ray;
    ray = (*ray).lastRay;
    free(oldRay);
    
    }

}


RD_ray* RD_castRayUpper(double rayAng, double startX, double startY) {
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
    bool lastWasX = 0;
    RD_ray* ray = (RD_ray*)malloc(sizeof(RD_ray));
    
    while (true) {
        if (mapX < 0 || mapX > MP_map.width - 1 || mapY < 0 || mapY > MP_map.height - 1) { //void

            (*ray).wallType = 100;
            (*ray).lastWasX = 0;
            (*ray).texPosX = 0;
            return ray;
        }
        
        int wall = abs(MP_map.map[mapX + (mapY * MP_map.width)]);
        if (wall != 0 && wall <= 16) { //tall
            
            (*ray).dist = curDist;
            (*ray).wallType = wall;
            (*ray).lastWasX = lastWasX;

            //check if texture needs to be flipped
            if (lastWasX) {
                double posY = startY + (curDist / yStep) * yDir;
                if (xDir < 0) {
                    (*ray).texPosX = (posY - (int)posY);
                }
                else {
                    (*ray).texPosX = (1 - (posY - (int)posY));
                }
            }
            else {
                double posX = startX + (curDist / xStep) * xDir;
                if (yDir < 0) {
                    (*ray).texPosX = (1 - (posX - (int)posX));
                }
                else {
                    (*ray).texPosX = (posX - (int)posX);
                }
            }

            return ray;
        }

        if (rayX < rayY) {
            curDist = rayX;
            rayX += xStep;
            mapX += xDir;
            lastWasX = 1;
        }
        else {
            curDist = rayY;
            rayY += yStep;
            mapY += yDir;
            lastWasX = 0;
        }
    }
}

RD_ray* RD_castRay(double rayAng, double startX, double startY, bool doDraw) {
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
    bool lastWasX = 0;
    RD_ray* curPtr = NULL;
    RD_ray* ray;

    while (true) {
        if (mapX < 0 || mapX > MP_map.width - 1 || mapY < 0 || mapY > MP_map.height - 1) { //void
            if (doDraw) {return ray;}
            ray = (RD_ray*)malloc(sizeof(RD_ray));
            (*ray).lastRay = curPtr;
            
            (*ray).dist = curDist;
            (*ray).wallType = 100;
            (*ray).lastWasX = 0;
            (*ray).texPosX = 0;
            return ray;
        }

        if (doDraw)
        {
            double x = startX + (curDist / xStep) * xDir;
            double y = startY + (curDist / yStep) * yDir;
            RD_drawPoint(x, y, 0xFFFFFFFF);
        }
        
        int wall = abs(MP_map.map[mapX + (mapY * MP_map.width)]);
        if (wall != 0) {
            if (!doDraw) {
                ray = (RD_ray*)malloc(sizeof(RD_ray));
                (*ray).lastRay = curPtr;
                curPtr = ray;
            
                (*ray).dist = curDist;
                (*ray).wallType = wall;
                (*ray).lastWasX = lastWasX;

                //check if texture needs to be flipped
                if (lastWasX) {
                    double posY = startY + (curDist / yStep) * yDir;
                    if (xDir < 0) {
                        (*ray).texPosX = (posY - (int)posY);
                    }
                    else {
                        (*ray).texPosX = (1 - (posY - (int)posY));
                    }
                }
                else {
                    double posX = startX + (curDist / xStep) * xDir;
                    if (yDir < 0) {
                        (*ray).texPosX = (1 - (posX - (int)posX));
                    }
                    else {
                        (*ray).texPosX = (posX - (int)posX);
                    }
                }
            }

            if (wall <= 32) {return ray;} //return if solid
        }

        if (rayX < rayY) {
            curDist = rayX;
            rayX += xStep;
            mapX += xDir;
            lastWasX = 1;
        }
        else {
            curDist = rayY;
            rayY += yStep;
            mapY += yDir;
            lastWasX = 0;
        }
    }
}


void RD_drawRay() {
    RD_castRay(PL_player.rot, PL_player.x, PL_player.y, true);
}

void RD_drawRays() {
    double fovOffset = (RD_rend.fov / 2) * -1; //starting fov offset
    double fovStep = (RD_rend.fov / 2) / RD_rend.screenWidth;  //move ray angle by this much every pixel

    for (int x = 0; x < RD_rend.screenWidth; x += 1) {
        double plaAng = PL_player.rot + fovOffset + (fovStep * x);

        RD_castRay(plaAng, PL_player.x, PL_player.y, true);

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
            if (wallType == 1 || wallType == 17) {
                screenCol = RD_colors.color1;
            }
            else if (wallType == 2 || wallType == 34) {
                screenCol = RD_colors.color2;
            }
            else if (wallType == 3) {
                screenCol = RD_colors.color3;
            }
            else {
                screenCol = RD_colors.color0;
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

        RD_ray* rayList;

        RD_drawFloor(x);

        RD_ray* ray = RD_castRayUpper(plaAng, PL_player.x, PL_player.y);
        RD_drawWallSliceUpper(x, plaAng, true, ray);

        ray = RD_castRay(plaAng, PL_player.x, PL_player.y, false);
        RD_drawWallSliceLower(x, plaAng, true, ray);

        fovOffset += fovStep;
    }
}

void RD_drawWolf() {
    double plaXStep = RD_rend.plaW / (RD_rend.screenWidth - 1);

    for (int x = 0; x < RD_rend.screenWidth; x += 1) {
        double plaX = x * plaXStep;
        double plaAng = atan((((RD_rend.plaW * -1) * 0.5) + plaX) / RD_rend.plaDist) + PL_player.rot;

        RD_drawFloor(x);

        RD_ray* ray = RD_castRayUpper(plaAng, PL_player.x, PL_player.y);
        RD_drawWallSliceUpper(x, plaAng, false, ray);

        ray = RD_castRay(plaAng, PL_player.x, PL_player.y, false);
        RD_drawWallSliceLower(x, plaAng, false, ray);
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
    RD_rend.screenWidth = 853; // 1440// 1600// 800 // 1280// 640// 853
    RD_rend.screenHeight = 480; // 1080// 900// 450 // 720// 360// 480

    RD_rend.pixels = (uint32_t*)malloc(RD_rend.screenWidth * RD_rend.screenHeight * sizeof(uint32_t));

    RD_rend.plaDist = 1; //can be practically anything and curent renderer will still work
    RD_changeFov(1.745329); //1.745329 = 100, (M_PI * 2) / 4 or 1.570796 = 90

    RD_rend.fishEye = false;

    RD_colors.voidColor = 0x00000000;
    RD_colors.color0 = 0x222222FF;
    RD_colors.skyColor = 0x110033FF;
    RD_colors.floorColor = 0x222222FF;
    RD_colors.color1 = 0x990044FF;
    RD_colors.color2 = 0x004499FF;
    RD_colors.color3 = 0x994400FF;



    for (int y = 0; y < 16; y += 1) {
        for (int x = 0; x < 16; x += 1) {
            RD_textures.textures[0][x + (y * 16)] = (0xAF000000 * (x % 4 && y % 4)) + 0xFF;
        }
    }

    RD_textures.textures[1][0] = 0x00000000;
    for (int y = 0; y < 16; y += 1) {
        for (int x = 1; x < 16; x += 1) {
            RD_textures.textures[1][x + (y * 16)] = (RD_textures.textures[1][(x - 1) + ((y - 1) * 16)] >> 1) ^ (RD_textures.textures[1][(x - 1) + ((y - 1) * 16)]);
        }
    }

    for (int y = 0; y < 16; y += 1) {
        for (int x = 0; x < 16; x += 1) {
            RD_textures.textures[2][x + (y * 16)] = (0x2FC000FF * (x % 4 ^ y % 4)) + 0xFF;
        }
    }
}

void RD_destroy() {
    free(RD_rend.pixels);
}
