#ifndef __PACKET_UTILITIES_H
#define __PACKET_UTILITIES_H

#include <stdint.h>

union _cnv {
    double cnvD;
    float cnvF;
    uint32_t cnv32;
    uint16_t cnv16[4];
    uint8_t cnv8[8];
};

void cnv32_8(const uint32_t from, uint8_t *to);

void cnv16_8(const uint16_t from, uint8_t *to);

void cnv8_32(uint8_t *from, uint32_t *to);

void cnv8_16(uint8_t *from, uint16_t *to);

void cnv8_16LE(uint8_t *from, uint16_t *to);

void cnvF_8(const float from, uint8_t *to);

void cnv8_F(uint8_t *from, float *to);

void cnvD_8(const double from, uint8_t *to);

void cnv8_D(uint8_t *from, double *to);

uint16_t htons(uint16_t x);
uint16_t ntohs(uint16_t x);

#endif
