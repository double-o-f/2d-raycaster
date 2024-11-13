#include <stdlib.h> //malloc
#include <stdio.h> //FILE, fwrite, fread, fseek, fflush

#include "PL_player.h" //PL_player.(x, y, rot)

#include "MP_map.h"
struct MP_map_ MP_map;


void MP_saveMap() {
    FILE* filePtr = fopen(MP_map.name,"wb");
    size_t offset = 0;
    
    int dim[2] = {MP_map.width, MP_map.height};
    fwrite(dim, sizeof(dim), 1, filePtr);
    offset += sizeof(dim);
    fseek(filePtr, offset, SEEK_SET);

    double pos[3] = {PL_player.x, PL_player.y, PL_player.rot};
    fwrite(pos, sizeof(pos), 1, filePtr);
    offset += sizeof(pos);
    fseek(filePtr, offset, SEEK_SET);

    fwrite(MP_map.map, MP_map.width * MP_map.height * sizeof(int), 1, filePtr);
    fflush(filePtr);
    printf("map saved\n");
}

void MP_createMap(int width, int height, const char* name) {
    if (MP_map.map != NULL) {
        free(MP_map.map);
    }

    MP_map.name = name;
    MP_map.width = width;
    MP_map.height = height;
    MP_map.map = (int*)malloc(MP_map.width * MP_map.height * sizeof(int));

    for (int x = 0 ; x < MP_map.width ; x += 1) {
        MP_changeWall(x, 0);
    }
    for (int y = 1 ; y < (MP_map.height - 1) ; y += 1) {
        MP_changeWall(0, y);
        MP_changeWall(MP_map.width - 1, y);
    }
    for (int x = 0 ; x < MP_map.width ; x += 1) {
        MP_changeWall(x, MP_map.height - 1);
    }
}

void MP_loadMap(const char* fileName) {
    if (MP_map.map != NULL) {
        free(MP_map.map);
    }

    MP_map.name = fileName;
    FILE* filePtr = fopen(fileName, "rb");
    size_t offset = 0;

    fread(&MP_map.width, sizeof(int), 1, filePtr);
    offset += sizeof(int);
    fseek(filePtr, offset, SEEK_SET);

    fread(&MP_map.height, sizeof(int), 1, filePtr);
    offset += sizeof(int);
    fseek(filePtr, offset, SEEK_SET);

    fread(&PL_player.x, sizeof(double), 1, filePtr);
    offset += sizeof(double);
    fseek(filePtr, offset, SEEK_SET);

    fread(&PL_player.y, sizeof(double), 1, filePtr);
    offset += sizeof(double);
    fseek(filePtr, offset, SEEK_SET);

    fread(&PL_player.rot, sizeof(double), 1, filePtr);
    offset += sizeof(double);
    fseek(filePtr, offset, SEEK_SET);
    PL_setRot(PL_player.rot);

    MP_map.map = (int*)malloc(MP_map.width * MP_map.height * sizeof(int));
    fread(MP_map.map, MP_map.width * MP_map.height * sizeof(int), 1, filePtr);
}

void MP_changeWall(int x, int y) {
    if (MP_map.map[x + (y * MP_map.width)] >= 3) {
        MP_map.map[x + (y * MP_map.width)] = 0;
    }
    else {
        MP_map.map[x + (y * MP_map.width)] += 1;
    }
}

void MP_initLoadMap() {
    MP_loadMap("resource/maps/map.map");
}

void MP_destroy() {
    free(MP_map.map);
}
