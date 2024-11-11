#ifndef HEADER_ST_INCLUDED
#define HEADER_ST_INCLUDED

#include <stdbool.h> //bool
//#include <math.h> //
#include <time.h> //clock


struct ST_state_{
    bool quit;
    double fpsCap;
    clock_t time;
    double delta;
};
extern struct ST_state_ ST_state;


void ST_tickFpsCapped();

void ST_init();

#endif