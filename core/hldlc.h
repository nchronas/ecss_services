#ifndef __HLDLC_H
#define __HLDLC_H

#include <stdint.h>
#include "satellite.h"

#define HLDLC_START_FLAG        0x7E
#define HLDLC_CONTROL_FLAG      0x7D
#define HLDLC_STOP_FLAG         0x7C

SAT_returnState HLDLC_deframe(uint8_t *buf_in,
                              uint8_t *buf_out,
                              uint16_t size_in,
                              uint16_t *size_out);

SAT_returnState HLDLC_frame(uint8_t *buf_in, uint8_t *buf_out, uint16_t *size);

#endif
