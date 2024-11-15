#include <stdbool.h> //bool
#include <math.h> //sin, cos, fabs

#include "MT_math.h" //zeroOut 
#include "MP_map.h" //MP_map.(map, width)
#include "ST_state.h" //ST_state.delta

#include "PL_player.h"
struct PL_player_ PL_player;


bool PL_collisionCheck(double x, double y) {
    if (x < 0 || x > MP_map.width || y < 0 || y > MP_map.height) {return true;}
    return (MP_map.map[(int)x + ((int)y * MP_map.width)] == 0 ? false : true);
}


void PL_setPos(double x, double y) {
    PL_player.x = x;
    PL_player.y = y;
}

void PL_setRot(double rot) {
    PL_player.rot = rot;

    if (PL_player.rot > (M_PI * 2)) {
        PL_player.rot = 0;
    }
    else if (PL_player.rot < 0) {
        PL_player.rot = (M_PI * 2);
    }

    PL_player.pSin = sin(PL_player.rot);
    PL_player.pCos = cos(PL_player.rot);
    double norm = fabs(PL_player.pSin) + fabs(PL_player.pCos);
    PL_player.nSin = PL_player.pSin / norm;
    PL_player.nCos = PL_player.pCos / norm;

}


void PL_rotate(double rSpeed) {
    PL_setRot(PL_player.rot += rSpeed * ST_state.delta);
}

void PL_mouseRotate(double mRot) {
    PL_setRot(PL_player.rot += PL_player.mSpeed * mRot);
}


void PL_NCForward(double speed) {
    PL_player.x += cos(PL_player.rot) * speed * ST_state.delta;
    PL_player.y += sin(PL_player.rot) * speed * ST_state.delta;
}

void PL_NCbackward(double speed) {
    PL_player.x += cos(PL_player.rot) * speed * -1 * ST_state.delta;
    PL_player.y += sin(PL_player.rot) * speed * -1 * ST_state.delta;
}

void PL_NCLeft(double speed) {
    PL_player.x += sin(PL_player.rot) * speed * ST_state.delta;
    PL_player.y += cos(PL_player.rot) * speed * -1 * ST_state.delta;
}

void PL_NCRight(double speed) {
    PL_player.x += sin(PL_player.rot) * speed * -1 * ST_state.delta;
    PL_player.y += cos(PL_player.rot) * speed * ST_state.delta;
}


void PL_calcDir(double newXVelo, double newYVelo) {
    PL_player.xVelo = newXVelo;
    PL_player.yVelo = newYVelo;

    double rXVelo;
    double rYVelo;

    rXVelo = PL_player.xVelo * PL_player.nCos;
    rYVelo = PL_player.yVelo * PL_player.nSin;
    if (fabs(rXVelo) + fabs(rYVelo) > PL_player.maxVelo) {
        double vNorm = fabs(rXVelo) + fabs(rYVelo);
        PL_player.xVelo = (PL_player.xVelo / vNorm) * PL_player.maxVelo;
        PL_player.yVelo = (PL_player.yVelo / vNorm) * PL_player.maxVelo;
    }

    rXVelo = PL_player.xVelo * PL_player.nSin;
    rYVelo = PL_player.yVelo * PL_player.nCos;
    if (fabs(rXVelo) + fabs(rYVelo) > PL_player.maxVelo) {
        double vNorm = fabs(rXVelo) + fabs(rYVelo);
        PL_player.xVelo = (PL_player.xVelo / vNorm) * PL_player.maxVelo;
        PL_player.yVelo = (PL_player.yVelo / vNorm) * PL_player.maxVelo;
    }
}

void PL_forward(double accel) {
    double newXVelo = PL_player.xVelo + PL_player.nCos * accel * ST_state.delta;
    double newYVelo = PL_player.yVelo + PL_player.nSin * accel * ST_state.delta;

    PL_calcDir(newXVelo, newYVelo);
}

void PL_backward(double accel) {
    double newXVelo = PL_player.xVelo + PL_player.nCos * accel * ST_state.delta * -1;
    double newYVelo = PL_player.yVelo + PL_player.nSin * accel * ST_state.delta * -1;

    PL_calcDir(newXVelo, newYVelo);
}

void PL_left(double accel) {
    double newXVelo = PL_player.xVelo + PL_player.nSin * accel * ST_state.delta;
    double newYVelo = PL_player.yVelo + PL_player.nCos * accel * ST_state.delta * -1;

    PL_calcDir(newXVelo, newYVelo);
}

void PL_right(double accel) {
    double newXVelo = PL_player.xVelo + PL_player.nSin * accel * ST_state.delta * -1;
    double newYVelo = PL_player.yVelo + PL_player.nCos * accel * ST_state.delta;

    PL_calcDir(newXVelo, newYVelo);
}


void PL_move() {
    bool cX =  PL_collisionCheck(PL_player.x + PL_player.xVelo + 0.01, PL_player.y);
    bool cY =  PL_collisionCheck(PL_player.x                          , PL_player.y + PL_player.yVelo + 0.01);
    bool cXY = PL_collisionCheck(PL_player.x + PL_player.xVelo + 0.01, PL_player.y + PL_player.yVelo + 0.01);
    
        cX |=  PL_collisionCheck(PL_player.x + PL_player.xVelo - 0.01, PL_player.y);
        cY |=  PL_collisionCheck(PL_player.x                          , PL_player.y + PL_player.yVelo - 0.01);
        cXY |= PL_collisionCheck(PL_player.x + PL_player.xVelo - 0.01, PL_player.y + PL_player.yVelo - 0.01);


    if (!cX && !cY && !cXY) { //no collision
        PL_player.x += PL_player.xVelo;
        PL_player.y += PL_player.yVelo;
    }
    //else if (!cX && !cY && cXY) { //collision on XY only
    //    if (PL_player.x + PL_player.xVelo < PL_player.x) {PL_player.x = (int)PL_player.x + 0.3;}
    //    else {PL_player.x = ((int)PL_player.x) + 1 - 0.3;}
    //    PL_player.xVelo = 0;
    //
    //    if (PL_player.y + PL_player.yVelo < PL_player.y) {PL_player.y = (int)PL_player.y + 0.3;}
    //    else {PL_player.y = ((int)PL_player.y) + 1 - 0.3;}
    //    PL_player.yVelo = 0;
    //}
    else if (!cX) { //no X collision
        PL_player.x += PL_player.xVelo; //apply X velo
        if (PL_player.y + PL_player.yVelo < PL_player.y) {PL_player.y = (int)PL_player.y + 0.01;}
        else if (PL_player.y + PL_player.yVelo > PL_player.y) {PL_player.y = ((int)PL_player.y) + 1 - 0.01;}
        PL_player.yVelo = 0;
    }
    else if (!cY) { //no X collision
        PL_player.y += PL_player.yVelo; //apply Y velo
        if (PL_player.x + PL_player.xVelo < PL_player.x) {PL_player.x = (int)PL_player.x + 0.01;}
        else if (PL_player.x + PL_player.xVelo > PL_player.x) {PL_player.x = ((int)PL_player.x) + 1 - 0.01;}
        PL_player.xVelo = 0;
    }
    else { //collision on X and Y but not XY
        if (PL_player.y + PL_player.yVelo < PL_player.y) {PL_player.y = (int)PL_player.y + 0.01;}
        else if (PL_player.y + PL_player.yVelo > PL_player.y) {PL_player.y = ((int)PL_player.y) + 1 - 0.01;}
        PL_player.yVelo = 0;
        
        if (PL_player.x + PL_player.xVelo < PL_player.x) {PL_player.x = (int)PL_player.x + 0.01;}
        else if (PL_player.x + PL_player.xVelo > PL_player.x) {PL_player.x = ((int)PL_player.x) + 1 - 0.01;}
        PL_player.xVelo = 0;
    }
}

void PL_friction(double friction) {
    double norm = fabs(PL_player.xVelo) + fabs(PL_player.yVelo);

    MT_zeroOut(&PL_player.xVelo, friction * fabs(PL_player.xVelo / norm) * ST_state.delta);
    MT_zeroOut(&PL_player.yVelo, friction * fabs(PL_player.yVelo / norm) * ST_state.delta);
}


void PL_moveStuff() {
    if (!PL_player.noclip) {
        PL_move();
        PL_friction(PL_player.friction);
    }
}


void PL_stats() {
    PL_player.maxVelo = 0.1;
    PL_player.accel = 0.4;
    PL_player.friction = 0.2;
    
    PL_player.noclip = false;
    PL_player.ncSpeed = 3;

    PL_player.rSpeed = 3;
    PL_player.mSpeed = 0.003;
}

void PL_init() {
    PL_player.x = 0; //set by MP_loadMap()
    PL_player.y = 0; //set by MP_loadMap()
    PL_player.rot = 0; //set by MP_loadMap()

    PL_player.xVelo = 0;
    PL_player.yVelo = 0;

    PL_setRot(PL_player.rot); //sets pSin pCos nSin nCos
    
    PL_stats();
}
