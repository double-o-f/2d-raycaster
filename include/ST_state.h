#ifndef HEADER_ST_INCLUDED
#define HEADER_ST_INCLUDED

#include <stdbool.h> //bool
//#include <math.h> //
#include <time.h> //clock


typedef struct {
    bool quit;
    double fpsCap;
    clock_t time;
    double delta;
} ST_state_;

extern ST_state_ ST_state;


void ST_tickFpsCapped();

void ST_tick();

void ST_init();

#endif