#ifndef __SATELLITE_H
#define __SATELLITE_H

#include "satellite_ids.h"
#include <stdbool.h>
#include <stddef.h>

//#define C_ASSERT(e)    ((e) ? (true) : (services_error_handler(__FILE_ID__, __LINE__, #e)))
#define C_ASSERT(e)    ((e) ? (true) : (services_error_handler(1, 1, #e)))


#define PKT_TIMEOUT 60000 /*in mseconds*/
#define PKT_NORMAL  198   /*MAX_PKT_DATA*/

#define POOL_PKT_SIZE         4
#define POOL_PKT_EXT_SIZE     0
#define POOL_PKT_TOTAL_SIZE   4

#define MAX_APP_ID      20
#define MAX_SERVICES    20
#define MAX_SUBTYPES    26

typedef enum {
    EPS_APP_ID      = _EPS_APP_ID_,
    COMMS_APP_ID    = _COMMS_APP_ID_,
    ADB_APP_ID      = _ADB_APP_ID_,
    GND_APP_ID      = _GND_APP_ID_,
    DBG_APP_ID      = _DBG_APP_ID_,
    LAST_APP_ID     = _LAST_APP_ID_
}TC_TM_app_id;

typedef enum {
    EPS_DEV_ID              =  1,
    COMMS_DEV_ID            =  2,
    ADCS_DEV_ID             =  3,
    EPS_BUS_DEV_ID          =  4,
    ADB_BUS_DEV_ID          =  5,
    COMMS_BUS_DEV_ID        =  6,
    ADCS_BUS_DEV_ID         =  7,
    EPS_DBG_DEV_ID          =  8,
    COMMS_DBG_DEV_ID        =  9,
    ADCS_DBG_DEV_ID         = 10,
    ADB_DBG_DEV_ID          = 11,
    EPS_OBC_MON_DEV_ID      = 12,
    EPS_COMMS_MON_DEV_ID    = 13,
    EPS_ADCS_MON_DEV_ID     = 14,
    EPS_SU_MON_DEV_ID       = 15,
    EPS_DC_MON_DEV_ID       = 16,
    EPS_UR_MON_DEV_ID       = 17,
    SOL_YP_MON_DEV_ID       = 18,
    SOL_YM_MON_DEV_ID       = 19,
    SOL_XP_MON_DEV_ID       = 20,
    SOL_XM_MON_DEV_ID       = 21,
    SOL_YP_TEMP_DEV_ID      = 22,
    SOL_YM_TEMP_DEV_ID      = 23,
    SOL_XP_TEMP_DEV_ID      = 24,
    SOL_XM_TEMP_DEV_ID      = 25,
    BATT_CHARGE_DEV_ID      = 26,
    EPS_FRAM_DEV_ID         = 27,
    ADB_MON_DEV_ID          = 28,
    ADB_TEMP_DEV_ID         = 29,
    OBC_MON_DEV_ID          = 30,
    OBC_TEMP_DEV_ID         = 31,
    OBC_FRAM_DEV_ID         = 32,
    ADCS_1_MON_DEV_ID       = 33,
    ADCS_2_MON_DEV_ID       = 34,
    ADCS_3_MON_DEV_ID       = 35,
    ADCS_4_MON_DEV_ID       = 36,
    ADCS_TEMP_DEV_ID        = 37,
    ADCS_FRAM_DEV_ID        = 38,
    LAST_DEV_ID             = 39
}dev_id;

typedef enum {
    EPS_INA_OBC_CUR_PAR_ID             =  0,
    EPS_INA_OBC_CUR_ARR_PAR_ID         =  1,
    EPS_INA_OBC_VLT_PAR_ID             =  2,
    EPS_INA_OBC_CUR_MON_HL_PAR_ID      =  3,
    EPS_INA_OBC_CUR_MON_LL_PAR_ID      =  4,
    LAST_PAR_ID                        =  5
}par_id;

typedef enum {
    TEST_VAULT_ID           =  0,
    LAST_VAULT_ID           =  1
}vault_id;

typedef enum {
    TEST_MON_ID             =  0,
    LAST_MON_ID             =  1
}mon_id;

typedef enum {
    SATR_PKT_ILLEGAL_APPID     = 0,
    SATR_PKT_INV_LEN           = 1,
    SATR_PKT_INC_CRC           = 2,
    SATR_PKT_ILLEGAL_PKT_TP    = 3,
    SATR_PKT_ILLEGAL_PKT_STP   = 4,
    SATR_PKT_ILLEGAL_APP_DATA  = 5,
    SATR_OK                    = 6,
    SATR_ERROR                 = 7,
    SATR_EOT                   = 8,
    SATR_CRC_ERROR             = 9,
    SATR_PKT_ILLEGAL_ACK       = 10,
    SATR_ALREADY_SERVICING     = 11,
    SATR_MS_MAX_FILES          = 12,
    SATR_PKT_INIT              = 13,
    SATR_INV_STORE_ID          = 14,
    SATR_INV_DATA_LEN          = 15,
    SATR_QUEUE_FULL            = 16,
    SATR_UNINIT                = 17,
    /*LAST*/
    SATR_LAST                  = 18
}SAT_returnState;

typedef enum {
    HEALTH_REP      = 1,
    LAST_STRUCT_ID  = 2
}HK_struct_id;

#endif
