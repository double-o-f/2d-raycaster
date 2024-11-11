#ifndef HEADER_PL_INCLUDED
#define HEADER_PL_INCLUDED

#include <stdbool.h> //bool
#include <math.h> //sin, cos, fabs

#include "MT_math.h" //zeroOut 
#include "MP_map.h" //MP_map.(map, width)
#include "ST_state.h" //ST_state.delta


struct PL_player_{
    double x;
    double y;

    double maxVelo;
    double xVelo;
    double yVelo;
    double accel;
    double friction;

    bool noclip;
    double ncSpeed;

    double rot;
    double rSpeed;
    double pSin;
    double pCos;
    double nSin;
    double nCos;
    double mSpeed;
};
extern struct PL_player_ PL_player;


bool PL_collisionCheck(double x, double y);


void PL_setPos(double x, double y);

void PL_setRot(double rot);


void PL_rotate(double rSpeed);

void PL_mouseRotate(double mRot);


void PL_NCForward(double speed);

void PL_NCbackward(double speed);

void PL_NCLeft(double speed);

void PL_NCRight(double speed);


void PL_calcDir(double newXVelo, double newYVelo);

void PL_forward(double accel);

void PL_backward(double accel);

void PL_left(double accel);

void PL_right(double accel);


void PL_move();

void PL_friction(double friction);


void PL_moveStuff();


void PL_stats();

void PL_init();

#endif
