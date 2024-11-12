#include <stdlib.h> //exit()
#include <stdio.h> //printf

#include "PL_player.h"
#include "MP_map.h"


int main(int argc, char const *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "too few arguments\n");
        exit(EXIT_FAILURE);
    }
    if (argc > 4) {
        fprintf(stderr, "too many arguments\n");
        exit(EXIT_FAILURE);
    }
    
    PL_init();
    PL_player.x = 2;
    PL_player.y = 2;
    PL_setRot(0);
    
    MP_createMap(atoi(argv[1]), atoi(argv[2]), argv[3]);
    MP_saveMap();

    printf("created map \"%s\" with size %sx%s\n", argv[3], argv[1], argv[2]);

    exit(EXIT_SUCCESS);
}
