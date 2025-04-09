#include <stdlib.h> //exit()
#include <stdio.h> //printf

#include "RC_math.h"
#include "RC_player.h"
#include "RC_map.h"
#include "RC_rend.h"
#include "RC_window.h"
#include "RC_input.h"
#include "RC_state.h"
#include "RC_ui.h"

int main(int argc, char const *argv[]) {
    printf("%s\n", ":)");
    
    //need to call all of these or something will brake
    PL_init();

    if (argc < 2) {
        MP_initLoadMap(); //need call after PL_init()
        printf("loaded resource/maps/map.map\n");
    }
    else if (argc == 2) {
        MP_loadMap(argv[1]); //need call after PL_init()
    }
    else if (argc > 2) {
        fprintf(stderr, "too many arguments\n");
        exit(EXIT_FAILURE);
    }

    RD_init();
    WI_init(); //need call after RD_init()

    IN_init();
    ST_init();
    UI_init();


    while (!ST_state.quit){
        
        ST_tickFpsCapped(); //if time since last call is < time to stay at frame rate limit wait
        //ST_tick();
        
        if (1 / ST_state.delta < 59) {
            printf("%f\n" , 1 / ST_state.delta);
        }

        IN_checkInputs(); //check inputs and do some game logic

        PL_moveStuff(); //player physics

        RD_drawStuff(); //render to pixel buffer

        WI_draw(); //draw pixel buffer to window
    }

    IN_destroy();
    WI_destroy();
    RD_destroy();
    MP_destroy();

    exit(EXIT_SUCCESS);
}
