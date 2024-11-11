#ifndef HEADER_MT_INCLUDED
#define HEADER_MT_INCLUDED

#include <math.h> //sqrtf, powf
#include <stdint.h> //uint32_t

uint32_t MT_setColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A);

double MT_getDist(double x1, double y1, double x2, double y2);

int MT_getSign(double num);

void MT_zeroOut(double* pVal, double step);

#endif
