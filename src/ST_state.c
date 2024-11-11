#include <stdbool.h> //bool
//#include <math.h> //
#include <time.h> //clock

#include "ST_state.h"
struct ST_state_ ST_state;


void ST_tickFpsCapped() {
    while (clock() - ST_state.time < ST_state.fpsCap) {}
    //printf("%ld\n", clock() - state.time);
    ST_state.delta = (double)(clock() - ST_state.time) / CLOCKS_PER_SEC; //1000000
    ST_state.time = clock();
}

void ST_init() {
    ST_state.quit = false;
    ST_state.fpsCap = (CLOCKS_PER_SEC / 60); //1000000
    ST_state.time = clock();
    ST_state.delta = 0; //updated when ST_tickFpsCapped() is called
}
