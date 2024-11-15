#include <math.h> //sqrtf, powf
#include <stdint.h> //uint32_t

uint32_t MT_setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    uint32_t col = r;
    col = (col << 8) + g;
    col = (col << 8) + b;
    col = (col << 8) + a;
    return col;
}

double MT_getDist(double x1, double y1, double x2, double y2) {
    return sqrtf(( powf(x2 - x1, 2) ) + ( powf(y2 - y1, 2) ));
}

int MT_getSign(double num) {
    return (num > 0 ? 1 : (num < 0 ? -1 : 0));
}

void MT_zeroOut(double* pVal, double step) {
    if (*pVal == 0) {return;}
    double oldVal = *pVal;
    step *= MT_getSign(*pVal) * -1;
    *pVal += step;
    if (MT_getSign(*pVal) != MT_getSign(oldVal)) {*pVal = 0;}
}
