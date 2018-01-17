#ifndef __PKT_POOL_H
#define __PKT_POOL_H

#include <stdint.h>
#include "services.h"
#include "system.h"

tc_tm_pkt * get_pkt(uint16_t size);

SAT_returnState free_pkt(tc_tm_pkt *pkt);

uint8_t is_free_pkt(tc_tm_pkt *pkt);

SAT_returnState pkt_pool_INIT();

void pkt_pool_IDLE(uint32_t tmp_time);

#endif
