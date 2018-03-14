#include "packet_utilities.h"

// need to check endiannes
void cnv32_8(const uint32_t from, uint8_t *to) {

    union _cnv cnv;

    cnv.cnv32 = from;
    to[0] = cnv.cnv8[0];
    to[1] = cnv.cnv8[1];
    to[2] = cnv.cnv8[2];
    to[3] = cnv.cnv8[3];
}

void cnv16_8(const uint16_t from, uint8_t *to) {

    union _cnv cnv;

    cnv.cnv16[0] = from;
    to[0] = cnv.cnv8[0];
    to[1] = cnv.cnv8[1];

}

void cnv8_32(uint8_t *from, uint32_t *to) {

    union _cnv cnv;

    cnv.cnv8[3] = from[3];
    cnv.cnv8[2] = from[2];
    cnv.cnv8[1] = from[1];
    cnv.cnv8[0] = from[0];
    *to = cnv.cnv32;

}

void cnv8_16LE(uint8_t *from, uint16_t *to) {

    union _cnv cnv;

    cnv.cnv8[1] = from[1];
    cnv.cnv8[0] = from[0];
    *to = cnv.cnv16[0];
}

void cnv8_16(uint8_t *from, uint16_t *to) {

    union _cnv cnv;

    cnv.cnv8[1] = from[0];
    cnv.cnv8[0] = from[1];
    *to = cnv.cnv16[0];
}


void cnvF_8(const float from, uint8_t *to) {

    union _cnv cnv;

    cnv.cnvF = from;
    to[0] = cnv.cnv8[0];
    to[1] = cnv.cnv8[1];
    to[2] = cnv.cnv8[2];
    to[3] = cnv.cnv8[3];
}

void cnv8_F(uint8_t *from, float *to) {

    union _cnv cnv;

    cnv.cnv8[3] = from[3];
    cnv.cnv8[2] = from[2];
    cnv.cnv8[1] = from[1];
    cnv.cnv8[0] = from[0];
    *to = cnv.cnvF;

}

void cnvD_8(const double from, uint8_t *to) {

    union _cnv cnv;

    cnv.cnvD = from;
    to[0] = cnv.cnv8[0];
    to[1] = cnv.cnv8[1];
    to[2] = cnv.cnv8[2];
    to[3] = cnv.cnv8[3];
    to[4] = cnv.cnv8[4];
    to[5] = cnv.cnv8[5];
    to[6] = cnv.cnv8[6];
    to[7] = cnv.cnv8[7];
}

void cnv8_D(uint8_t *from, double *to) {

    union _cnv cnv;

    cnv.cnv8[7] = from[7];
    cnv.cnv8[6] = from[6];
    cnv.cnv8[5] = from[5];
    cnv.cnv8[4] = from[4];
    cnv.cnv8[3] = from[3];
    cnv.cnv8[2] = from[2];
    cnv.cnv8[1] = from[1];
    cnv.cnv8[0] = from[0];
    *to = cnv.cnvD;

}
