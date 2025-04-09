#ifndef HEADER_RC_MP_INCLUDED
#define HEADER_RC_MP_INCLUDED

#include <stdlib.h> //malloc
#include <stdio.h> //FILE, fwrite, fread, fseek, fflush

#include "RC_player.h" //PL_player.(x, y, rot)


typedef struct {
    int width;
    int height;
    int* map;
    const char* name;
} MP_map_;

extern MP_map_ MP_map;


void MP_saveMap();

void MP_createMap(int width, int height, const char* name);

void MP_loadMap(const char* fileName);

void MP_changeWall(int x, int y);

void MP_initLoadMap();

void MP_destroy();

#endif
