#include <stdlib.h> //exit()
#include <stdio.h> //printf

#include "MT_math.h"
#include "PL_player.h"
#include "MP_map.h"
#include "RD_rend.h"
#include "WI_window.h"
#include "IN_input.h"
#include "ST_state.h"
#include "UI_ui.h"

int main(int argc, char const *argv[]) {
    printf("%s\n", ":)");
    

    //need to call all of these or something will brake
    PL_init();
    MP_initLoadMap(); //need call after PL_init()

    RD_init();
    WI_init(); //need call after RD_init()

    IN_init();
    ST_init();
    UI_init();


    while (!ST_state.quit){
        
        ST_tickFpsCapped(); //if time since last call is < time to stay at frame rate limit wait
        
        IN_checkInputs(); //check inputs and do some game logic

        PL_moveStuff(); //player physics

        RD_drawStuff(); //render to pixel buffer

        WI_Draw(); //draw pixel buffer to window
    }

    WI_Destroy();

    exit(EXIT_SUCCESS);
}
