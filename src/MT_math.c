#include <math.h> //sqrtf, powf
#include <stdint.h> //uint32_t

uint32_t MT_setColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A) {
    uint32_t col = (16777216 * R) + (65536 * G) + (256 * B) + (1 * A);
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
