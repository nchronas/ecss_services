#include "subsystem.h"

SAT_returnState route_pkt(tc_tm_pkt *pkt) {

    SAT_returnState res;
    TC_TM_app_id id;

    if(!C_ASSERT(pkt != NULL &&
                 pkt->data != NULL) == true) {
                   return SATR_ERROR;
                 }
    if(!C_ASSERT(pkt->type == TC ||
                 pkt->type == TM) == true) {
                   return SATR_ERROR;
                 }
    if(!C_ASSERT(pkt->app_id < LAST_APP_ID &&
                 pkt->dest_id < LAST_APP_ID) == true) {
                   return SATR_ERROR;
                 }

    if(pkt->type == TC)         { id = pkt->app_id; }
    else if(pkt->type == TM)    { id = pkt->dest_id; }

    if(id == SYSTEM_APP_ID && pkt->ser_type == TC_TEST_SERVICE) {
        res = test_app(pkt);
    } else if(id == SYSTEM_APP_ID && pkt->ser_type == TC_HOUSEKEEPING_SERVICE) {
        res = hk_app(pkt);
    } else if(id != SYSTEM_APP_ID) {
        queuePush(pkt, 1);
    }

    return SATR_OK;
}
