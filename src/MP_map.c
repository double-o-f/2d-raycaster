#include <stdlib.h> //malloc
#include <stdio.h> //FILE, fwrite, fread, fseek, fflush

#include "PL_player.h" //PL_player.(x, y, rot)

#include "MP_map.h"
struct MP_map_ MP_map;


void MP_saveMap() {
    FILE* filePtr = fopen("resource/maps/map.map","wb");
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
}

void MP_loadMap(char* fileName) {
    FILE* filePtr = fopen(fileName, "rb");
    size_t offset = 0;

    fread(&MP_map.height, sizeof(int), 1, filePtr);
    offset += sizeof(int);
    fseek(filePtr, offset, SEEK_SET);

    fread(&MP_map.width, sizeof(int), 1, filePtr);
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
