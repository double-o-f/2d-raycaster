#ifndef HEADER_MP_INCLUDED
#define HEADER_MP_INCLUDED

#include <stdlib.h> //malloc
#include <stdio.h> //FILE, fwrite, fread, fseek, fflush

#include "PL_player.h" //PL_player.(x, y, rot)


struct MP_map_ {
    int width;
    int height;
    int* map;
    const char* name;
};
extern struct MP_map_ MP_map;


void MP_saveMap();

void MP_createMap(int width, int height, const char* name);

void MP_loadMap(const char* fileName);

void MP_changeWall(int x, int y);

void MP_initLoadMap();

void MP_destroy();

#endif
