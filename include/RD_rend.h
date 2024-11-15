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
};
extern struct RD_rend_ RD_rend;

typedef struct RD_ray_ RD_ray;


void RD_drawCrosshair();

void RD_drawPoint(double x, double y, uint32_t col);

//void RD_blackScreen();

void RD_fillScreen(uint32_t col);

void RD_drawFloor(int x);

void RD_drawVertLine(int x, int startY, int endY, uint32_t col);

void RD_drawWallSliceUpper(int x, double plaAng, bool fish, RD_ray* ray);

void RD_drawWallSliceLower(int x, double plaAng, bool fish, RD_ray* ray);

RD_ray* RD_castRayUpper(double rayAng, double startX, double startY);

RD_ray* RD_castRay(double rayAng, double startX, double startY, bool doDraw);


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
