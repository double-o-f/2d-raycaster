#ifndef HEADER_RD_INCLUDED
#define HEADER_RD_INCLUDED

#include <stdlib.h> //malloc
#include <stdbool.h> //bool
#include <math.h> //sin, cos, tan, atan, sqrtf

#include "PL_player.h" //PL_player.(x, y, rot)
#include "MP_map.h" //MP_map.(map, width, height)


struct RD_rend_{
    uint16_t screenWidth;
    uint16_t screenHeight;

    uint32_t* pixels;

    double fov; //in radians rn
    double plaDist; //plane dist from player
    double plaW; //plane width

    bool fishEye;

    uint32_t voidColor;
    uint32_t skyColor;
    uint32_t floorColor;
    uint32_t color0;
    uint32_t color1;
    uint32_t color2;
    uint32_t color3;
};
extern struct RD_rend_ RD_rend;


void RD_drawCrosshair();

void RD_drawPoint(double x, double y, uint32_t col);

void RD_fillScreen(uint32_t col);

void RD_drawVertLine(int x, int startY, int endY, uint32_t col);

void RD_drawWallSlice(int x, int wallHeight, int wallType, uint32_t bright);


void RD_castRay(double rayAng, double startX, double startY, bool doDraw, double* posX, double* posY, double* dist, int* wallType, bool* lastWasX);


void RD_drawRay();

void RD_drawRays();


void RD_drawMap();

void RD_drawFish();

void RD_drawWolf();


void RD_drawStuff();


void RD_changeFov(double fov);

void RD_init();

void RD_destroy();

#endif
