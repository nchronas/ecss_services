#include "mass_storage_service.h"

#include "su_mnlp.h"
#include "pkt_pool.h"

#include <stdlib.h>
#include <string.h>
#include "fatfs.h"
#include "service_utilities.h"

#define MS_SD_PATH "0:"

#define MS_SU_LOG          "/SU_LOG"
#define MS_WOD_LOG         "/WOD_LOG"
#define MS_EXT_WOD_LOG     "/EXT_WOD"
#define MS_SU_SCRIPT_1     "/SU_SCR_1/SCR1.bin"
#define MS_SU_SCRIPT_2     "/SU_SCR_2/SCR2.bin"
#define MS_SU_SCRIPT_3     "/SU_SCR_3/SCR3.bin"
#define MS_SU_SCRIPT_4     "/SU_SCR_4/SCR4.bin"
#define MS_SU_SCRIPT_5     "/SU_SCR_5/SCR5.bin"
#define MS_SU_SCRIPT_6     "/SU_SCR_6/SCR6.bin"
#define MS_SU_SCRIPT_7     "/SU_SCR_7/SCR7.bin"
#define MS_EVENT_LOG       "/EV_LOG"
#define MS_FOTOS           "/FOTOS"
#define MS_SCHS            "/SCHS"

#define MS_MAX_PATH             40 //random num
#define MS_MAX_FNAME            30 //random num
#define MS_MAX_LOG_FILE_SIZE    198 /*MAX_PKT_DATA*/ 
#define MS_MAX_SU_FILE_SIZE     2048 //2k
#define MS_FILE_SECTOR          512
#define MS_STORES               3
#define MS_SU_FSIZE             174
#define MS_MIN_SU_FILE          1   //min is the header.
#define MAX_F_RETRIES           3

#undef __FILE_ID__
#define __FILE_ID__ 8

#define MS_ERR(res);    ms_debugging(res, __LINE__); return res + SATRF_OK;

struct _MS_data {
    FATFS Fs;
    uint8_t enabled;
    SAT_returnState last_err;
};

extern struct _MNLP_data MNLP_data;
extern SAT_returnState route_pkt(tc_tm_pkt *pkt);

extern uint32_t get_new_fileId(MS_sid sid);

static struct _MS_data MS_data = { .enabled = true, \
                                   .last_err = FR_OK } ;

void ms_debugging(FRESULT res, uint16_t l) {

    MS_data.last_err = res;
    event_ms_err(res, l);

}

/**
 * @brief      { entry point for incoming packets. }
 *
 * @param      pkt   The pkt
 *
 * @return     { description_of_the_return_value }
 */
SAT_returnState mass_storage_app(tc_tm_pkt *pkt) {

    SAT_returnState res = SATR_ERROR;

    if(!C_ASSERT(pkt != NULL && pkt->data != NULL) == true) { return SATR_ERROR; }
    if(!C_ASSERT(pkt->ser_type == TC_MASS_STORAGE_SERVICE) == true) { return SATR_ERROR; }
    if(!C_ASSERT(pkt->ser_subtype == TC_MS_DISABLE || \
                 pkt->ser_subtype == TC_MS_ENABLE || \
                 pkt->ser_subtype == TC_MS_DELETE || \
                 pkt->ser_subtype == TC_MS_FORMAT || \
                 pkt->ser_subtype == TC_MS_REPORT || \
                 pkt->ser_subtype == TC_MS_DOWNLINK || \
                 pkt->ser_subtype == TC_MS_UPLINK) == true) { return SATR_ERROR; }

    MS_sid sid = (MS_sid)pkt->data[0];

    if(!C_ASSERT(sid < LAST_SID) == true) { return SATR_ERROR; }

    if(pkt->ser_subtype == TC_MS_DISABLE) {

        power_control_api(OBC_SD_DEV_ID, P_OFF);
        MS_data.enabled = false;
        res = SATR_OK;

    } else if(pkt->ser_subtype == TC_MS_ENABLE) {

        power_control_api(OBC_SD_DEV_ID, P_ON);
        HAL_sys_delay(1);
        res = mass_storage_init();
        if(res == SATR_OK) { MS_data.enabled = true; }

    } else if(pkt->ser_subtype == TC_MS_FORMAT) {

        res = mass_storage_FORMAT();
        if(res == SATR_OK) { 

            if(mass_storage_dirCheck() != SATR_OK) { res = SATRF_DIR_ERROR; }

        }
    } else if(pkt->ser_subtype == TC_MS_DELETE) {

        uint32_t to;
        MS_mode mode;

        if(sid <= SU_SCRIPT_7) {

            for(uint8_t i = 0; i < MAX_F_RETRIES; i++) {
                if((res = mass_storage_delete_su_scr(sid)) != SATRF_LOCKED) { break; }
                HAL_sys_delay(1);
            }

        } else {

            mode = pkt->data[1];
            cnv8_32(&pkt->data[2], &to);

            for(uint8_t i = 0; i < MAX_F_RETRIES; i++) {
                if((res = mass_storage_delete_api(sid, to, mode)) != SATRF_LOCKED) { break; }
                HAL_sys_delay(1);
            }
        }

    } else if(pkt->ser_subtype == TC_MS_REPORT) {

        for(uint8_t i = 0; i < MAX_F_RETRIES; i++) {
            if((res = mass_storage_report_api(pkt, sid)) != SATRF_LOCKED) { break; }
            HAL_sys_delay(1);
        }

    } else if(pkt->ser_subtype == TC_MS_DOWNLINK) {

        uint32_t file;

        cnv8_32(&pkt->data[1], &file);
        for(uint8_t i = 0; i < MAX_F_RETRIES; i++) {
            if((res = mass_storage_downlink_api(pkt, sid, file)) != SATRF_LOCKED) { break; }
            HAL_sys_delay(1);
        }

    } else if(pkt->ser_subtype == TC_MS_UPLINK) {

        uint16_t size = pkt->len -1;

        for(uint8_t i = 0; i < MAX_F_RETRIES; i++) {
            if((res = mass_storage_storeFile(sid, 0,&pkt->data[1], &size)) != SATRF_LOCKED) { break; }
            HAL_sys_delay(1);
        }

    } else { return SATR_ERROR; }

    pkt->verification_state = res;

    if(res != SATR_OK) {
        return SATR_ERROR;    
    }
    return SATR_OK; 
}

/**
 * @brief      { function_description }
 *
 * @param[in]  sid   The sid
 *
 * @return     { description_of_the_return_value }
 */
SAT_returnState mass_storage_delete_su_scr(MS_sid sid) {

    FRESULT res;
    FILINFO fno;
    uint8_t path[MS_MAX_PATH];

    if(!C_ASSERT(MS_data.enabled == true) == true) { return SATR_SD_DISABLED; }
    if(!C_ASSERT(sid < SU_SCRIPT_7) == true) { return SATR_ERROR; }

    if(sid == SU_SCRIPT_1) { strncpy((char*)path, MS_SU_SCRIPT_1, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_2) { strncpy((char*)path, MS_SU_SCRIPT_2, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_3) { strncpy((char*)path, MS_SU_SCRIPT_3, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_4) { strncpy((char*)path, MS_SU_SCRIPT_4, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_5) { strncpy((char*)path, MS_SU_SCRIPT_5, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_6) { strncpy((char*)path, MS_SU_SCRIPT_6, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_7) { strncpy((char*)path, MS_SU_SCRIPT_7, MS_MAX_PATH); }

    if((res = f_stat((char*)path, &fno)) != FR_OK) { MS_ERR(res) } 


    if((res = f_unlink((char*)path)) != FR_OK)     { return res + SATRF_OK; } 

    //function
    MNLP_data.su_scripts[(uint8_t)sid-1].valid_logi = false;

    return SATR_OK;
}

/*delete handles deletion of mass storage. sid denotes the store id.*/
/*if to is 0: it deletes every file of the sid else it deletes every file which time is lower then the time denoted in to*/
SAT_returnState mass_storage_delete_api(MS_sid sid, uint32_t to, MS_mode mode) {

    FRESULT res;
    FILINFO fno;
    DIR dir;
    uint8_t *fn;
    uint8_t path[MS_MAX_PATH];
    uint8_t temp_path[MS_MAX_PATH];
    uint16_t i;

    if(!C_ASSERT(MS_data.enabled == true) == true) { return SATR_SD_DISABLED; }
    if(!C_ASSERT(sid == SU_LOG || sid == WOD_LOG ||sid == EVENT_LOG || sid == FOTOS || \
                (sid <= SU_SCRIPT_7 && mode == SPECIFIC)) == true) { return SATR_ERROR; }

    if(sid == SU_LOG)           { strncpy((char*)path, MS_SU_LOG, MS_MAX_PATH); }
    else if(sid == WOD_LOG)     { strncpy((char*)path, MS_WOD_LOG, MS_MAX_PATH); }
    else if(sid == EXT_WOD_LOG) { strncpy((char*)path, MS_EXT_WOD_LOG, MS_MAX_PATH); }
    else if(sid == EVENT_LOG)   { strncpy((char*)path, MS_EVENT_LOG, MS_MAX_PATH); }
    else if(sid == FOTOS)       { strncpy((char*)path, MS_FOTOS, MS_MAX_PATH); }

    if((res = f_opendir(&dir, (char*)path)) != FR_OK) { return res + SATRF_OK; }  //add more error checking

    for (i = 0; i < MS_MAX_FILES; i++) {

        if((res = f_readdir(&dir, &fno)) != FR_OK) {
            f_closedir(&dir);
            return res + SATRF_OK; 
        }  /* Break on error */
        else if(fno.fname[0] == 0) { break; }  /* Break on end of dir */
        else if (fno.fname[0] == '.') { continue; }             /* Ignore dot entry */

        fn = (uint8_t*)fno.fname;

        uint32_t ret = strtol((char*)fn, NULL, 10);
        if((mode == ALL) || (mode == TO && ret <= to) || (mode == SPECIFIC && ret == to)) {

            sprintf(temp_path,"%s/%s", path, (char*)fn);

            if((res = f_stat((char*)temp_path, &fno)) != FR_OK) {
                f_closedir(&dir);
                return res + SATRF_OK; 
            }

            if((res = f_unlink((char*)temp_path)) != FR_OK)     { 
                f_closedir(&dir); 
                return res + SATRF_OK; 
            }

            if((mode == TO && ret == to) || (mode == SPECIFIC && ret == to)) { break; }

        }
    }
    f_closedir(&dir);

    if(i == MS_MAX_FILES - 1) { return SATR_MS_MAX_FILES; }
    
    return SATR_OK;
}

SAT_returnState mass_storage_downlink_api(tc_tm_pkt *pkt, MS_sid sid, uint32_t file) {

    uint16_t size;
    SAT_returnState res;
    TC_TM_app_id app_id;
    tc_tm_pkt *temp_pkt = 0;

    if(!C_ASSERT(MS_data.enabled == true) == true) { return SATR_SD_DISABLED; }
    if(!C_ASSERT(pkt != NULL && pkt->data != NULL) == true) { return SATR_ERROR; }

    app_id = (TC_TM_app_id)pkt->dest_id; //check if this is ok

    if(!C_ASSERT(sid < LAST_SID) == true)    { return SATR_ERROR; }

    mass_storage_crtPkt(&temp_pkt, app_id, MAX_PKT_EXT_DATA);

    res = mass_storage_downlinkFile(sid, file, temp_pkt->data, &size);

    if(res != SATR_OK)                              { free_pkt(temp_pkt); return res; }
    if(!C_ASSERT(size <= MAX_PKT_EXT_DATA) == true) { free_pkt(temp_pkt); return SATR_ERROR; }
    temp_pkt->len = size;

    mass_storage_updatePkt(temp_pkt, size, TC_MS_CONTENT);
    route_pkt(temp_pkt);

    return SATR_OK;
}

extern uint8_t uart_temp[];

SAT_returnState mass_storage_downlinkFile(MS_sid sid, uint32_t file, uint8_t *buf, uint16_t *size) {

    FIL fp;
    FRESULT res;
    uint16_t byteswritten;
    uint8_t path[MS_MAX_PATH];

    if(!C_ASSERT(MS_data.enabled == true) == true) { return SATR_SD_DISABLED; }
    if(!C_ASSERT(buf != NULL && size != NULL) == true)  { return SATR_ERROR; }
    if(!C_ASSERT(sid < LAST_SID) == true)               { return SATR_ERROR; }

    /*cp dir belonging to sid*/
    if(sid == SU_LOG)           { snprintf((char*)path, MS_MAX_PATH, "%s//%d", MS_SU_LOG, file); }
    else if(sid == WOD_LOG)     { snprintf((char*)path, MS_MAX_PATH, "%s//%d", MS_WOD_LOG, file); }
    else if(sid == EXT_WOD_LOG) { snprintf((char*)path, MS_MAX_PATH, "%s//%d", MS_EXT_WOD_LOG, file); }
    else if(sid == EVENT_LOG)   { snprintf((char*)path, MS_MAX_PATH, "%s//%d", MS_EVENT_LOG, file); }
    else if(sid == FOTOS)       { snprintf((char*)path, MS_MAX_PATH, "%s//%d", MS_FOTOS, file); }
    else if(sid == SU_SCRIPT_1) { strncpy((char*)path, MS_SU_SCRIPT_1, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_2) { strncpy((char*)path, MS_SU_SCRIPT_2, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_3) { strncpy((char*)path, MS_SU_SCRIPT_3, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_4) { strncpy((char*)path, MS_SU_SCRIPT_4, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_5) { strncpy((char*)path, MS_SU_SCRIPT_5, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_6) { strncpy((char*)path, MS_SU_SCRIPT_6, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_7) { strncpy((char*)path, MS_SU_SCRIPT_7, MS_MAX_PATH); }
    else if(sid == SCHS)        { snprintf((char*)path, MS_MAX_PATH, "%s//%d", MS_SCHS, file); }

    *size = MAX_PKT_EXT_DATA;

    if((res = f_open(&fp, (char*)path, FA_OPEN_EXISTING | FA_READ)) != FR_OK) { return res + SATRF_OK; } 

    res = f_read(&fp, buf, *size, (void *)&byteswritten);
    f_close(&fp);

    if(res != FR_OK) { return res + SATRF_OK; } 
    else if(byteswritten == 0) { return SATR_ERROR; } 
    *size = byteswritten;

    return SATR_OK;
}

SAT_returnState mass_storage_storeFile(MS_sid sid, uint32_t file, uint8_t *buf, uint16_t *size) {

    FIL fp;
    FRESULT res;
    FILINFO fno;

    uint16_t byteswritten;
    uint8_t path[MS_MAX_PATH];

    if(!C_ASSERT(MS_data.enabled == true) == true) { return SATR_SD_DISABLED; }
    if(!C_ASSERT(buf != NULL && size != NULL) == true)  { return SATR_ERROR; }
    //if(!C_ASSERT(*size > 0 && *size < _MAX_SS) == true) { return SATR_ERROR; }
    if(!C_ASSERT(sid <= LAST_SID) == true)              { return SATR_ERROR; }

    if(sid == SU_LOG)           { snprintf((char*)path, MS_MAX_PATH, "%s//%d", MS_SU_LOG, get_new_fileId(sid)); }
    else if(sid == WOD_LOG)     { snprintf((char*)path, MS_MAX_PATH, "%s//%d", MS_WOD_LOG, get_new_fileId(sid)); }
    else if(sid == EXT_WOD_LOG) { snprintf((char*)path, MS_MAX_PATH, "%s//%d", MS_EXT_WOD_LOG, get_new_fileId(sid)); }
    else if(sid == EVENT_LOG)   { snprintf((char*)path, MS_MAX_PATH, "%s//%d", MS_EVENT_LOG, get_new_fileId(sid)); }
    else if(sid == FOTOS)       { snprintf((char*)path, MS_MAX_PATH, "%s//%d", MS_FOTOS, get_new_fileId(sid)); }
    else if(sid == SU_SCRIPT_1) { strncpy((char*)path, MS_SU_SCRIPT_1, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_2) { strncpy((char*)path, MS_SU_SCRIPT_2, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_3) { strncpy((char*)path, MS_SU_SCRIPT_3, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_4) { strncpy((char*)path, MS_SU_SCRIPT_4, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_5) { strncpy((char*)path, MS_SU_SCRIPT_5, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_6) { strncpy((char*)path, MS_SU_SCRIPT_6, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_7) { strncpy((char*)path, MS_SU_SCRIPT_7, MS_MAX_PATH); }
    else if(sid == SCHS)        { snprintf((char*)path, MS_MAX_PATH, "%s//%d", MS_SCHS, file); }

    if(sid <= SU_SCRIPT_7) {
        res = f_stat((char*)path, &fno);
        if(res != FR_NO_FILE) { return SATRF_EXIST; }
    }

    if((res = f_open(&fp, (char*)path, FA_OPEN_ALWAYS | FA_WRITE)) != FR_OK) { return res + SATRF_OK; } 

    res = f_write(&fp, buf, *size, (void *)&byteswritten);
    f_close(&fp);

    if(res != FR_OK) { return res + SATRF_OK; } 
    else if(byteswritten == 0) { return SATR_ERROR; } 

    if(sid <= SU_SCRIPT_7) {
    //    TODO: TO Test SCRIPT UPDATE PROCEDURE
        MNLP_data.su_scripts[(uint8_t)sid-1].valid_logi = false; /*stops if is executing*/
        SAT_returnState sat_res = mass_storage_su_load_api( sid, MNLP_data.su_scripts[(uint8_t) sid - 1].file_load_buf);
        if(sat_res == SATR_ERROR || sat_res == SATR_CRC_ERROR){ 
            /*faled to re-read freshly uploaded script, ???*/
            return sat_res;
        }
        MNLP_data.su_scripts[(uint8_t) sid-1].valid_str = true;
        su_populate_header( &(MNLP_data.su_scripts[(uint8_t) sid - 1].scr_header), MNLP_data.su_scripts[(uint8_t) sid - 1].file_load_buf);
        MNLP_data.su_scripts[(uint8_t)sid-1].valid_logi = true;

    }
    
    return SATR_OK;
}

SAT_returnState mass_storage_report_api(tc_tm_pkt *pkt, MS_sid sid) {

    SAT_returnState res = SATR_ERROR;
    uint16_t size = 0;
    tc_tm_pkt *temp_pkt = 0;

    if(!C_ASSERT(MS_data.enabled == true) == true) { return SATR_SD_DISABLED; }
    if(!C_ASSERT(pkt != NULL && pkt->data != NULL) == true) { return SATR_ERROR; }

    TC_TM_app_id app_id = (TC_TM_app_id)pkt->dest_id; //check if this is ok

    mass_storage_crtPkt(&temp_pkt, app_id, PKT_NORMAL);

    if(sid <= SU_SCRIPT_7) {
        res = mass_storage_report_su_scr(sid, temp_pkt->data, &size);
    } else {
        uint32_t iter = 0;
        
        cnv8_32(&pkt->data[1], &iter);

        res = mass_storage_report(sid, temp_pkt->data, &size, &iter); 
    }

    if(!(res == SATR_OK || res == SATR_EOT || res == SATR_MS_MAX_FILES)) { free_pkt(temp_pkt); return res; }

    temp_pkt->len = size;

    mass_storage_updatePkt(temp_pkt, size, TM_MS_CATALOGUE_REPORT);
    route_pkt(temp_pkt);

    return SATR_OK;
}

SAT_returnState mass_storage_report(MS_sid sid, uint8_t *buf, uint16_t *size, uint32_t *iter) {

    DIR dir;
    FILINFO fno;
    FRESULT res = 0;
    uint32_t ret;
    uint8_t *fn;
    uint8_t start_flag = 0;
    uint8_t path[MS_MAX_PATH];
    uint8_t temp_path[MS_MAX_PATH];
    uint16_t i;

    if(!C_ASSERT(MS_data.enabled == true) == true) { return SATR_SD_DISABLED; }
    if(!C_ASSERT(buf != NULL && size != NULL && iter != NULL) == true)                            { return SATR_ERROR; }
    if(!C_ASSERT(*size == 0) == true)                                                             { return SATR_ERROR; }
    if(!C_ASSERT(sid == SU_LOG || sid == WOD_LOG || sid == EXT_WOD_LOG || sid == EVENT_LOG || sid == FOTOS) == true)    { return SATR_ERROR; }

    if(sid == SU_LOG)           { strncpy((char*)path, MS_SU_LOG, MS_MAX_PATH); }
    else if(sid == WOD_LOG)     { strncpy((char*)path, MS_WOD_LOG, MS_MAX_PATH); }
    else if(sid == EXT_WOD_LOG) { strncpy((char*)path, MS_EXT_WOD_LOG, MS_MAX_PATH); }
    else if(sid == EVENT_LOG)   { strncpy((char*)path, MS_EVENT_LOG, MS_MAX_PATH); }
    else if(sid == FOTOS)       { strncpy((char*)path, MS_FOTOS, MS_MAX_PATH); }

    if(*iter == 0) { start_flag = 1; }
    else { start_flag = 0; }

    if((res = f_opendir(&dir, (char*)path)) != FR_OK) { return res + SATRF_OK; }
    for (i = 0; i < MS_MAX_FILES; i++) {

        if((res = f_readdir(&dir, &fno)) != FR_OK) { f_closedir(&dir); return res + SATRF_OK; }  /* Break on error */
        else if(fno.fname[0] == 0)    { f_closedir(&dir); return SATR_EOT; }  /* Break on end of dir */
        else if (fno.fname[0] == '.') { continue; }             /* Ignore dot entry */

        fn = (uint8_t*)fno.fname; /*NOT _USE_LFN*/

        ret = strtol((char*)fn, NULL, 10);
        if(start_flag == 0 && *iter == ret) { start_flag = 1; }
        if(start_flag == 1) {

            sprintf(temp_path,"%s/%s", path, (char*)fn);
            if((res = f_stat(temp_path, &fno)) != FR_OK) { f_closedir(&dir); return res + SATRF_OK; } 

            cnv32_8(ret, &buf[(*size)]);
            *size += sizeof(uint32_t);

            cnv32_8(fno.fsize, &buf[(*size)]);
            *size += sizeof(uint32_t);

            cnv16_8(fno.fdate, &buf[(*size)]);
            *size += sizeof(uint16_t);

            cnv16_8(fno.ftime, &buf[(*size)]);
            *size += sizeof(uint16_t);
 
            if(*size >= MAX_PKT_DATA + (sizeof(uint32_t) * 4)) { f_closedir(&dir); return SATR_OK; }
        } 

    }
    f_closedir(&dir);
 
    //if(!C_ASSERT(*size != 0) == true) { return SATR_ERROR; }
    if(i == MS_MAX_FILES - 1) { return SATR_MS_MAX_FILES; }

    return SATR_OK;
}

SAT_returnState mass_storage_report_su_scr(MS_sid sid, uint8_t *buf, uint16_t *size) {

    FILINFO fno;
    FRESULT res;
    uint8_t path[MS_MAX_PATH];

    if(!C_ASSERT(MS_data.enabled == true) == true) { return SATR_SD_DISABLED; }
    if(!C_ASSERT(buf != NULL && size != NULL) == true)  { return SATR_ERROR; }
    if(!C_ASSERT(*size == 0) == true)                   { return SATR_ERROR; }
    if(!C_ASSERT(sid <= SU_SCRIPT_7) == true)           { return SATR_ERROR; }

    for(uint8_t i = SU_SCRIPT_1; i <= SU_SCRIPT_7; i++) {

        if(i == SU_SCRIPT_1)          { strncpy((char*)path, MS_SU_SCRIPT_1, MS_MAX_PATH); }
        else if(i == SU_SCRIPT_2)     { strncpy((char*)path, MS_SU_SCRIPT_2, MS_MAX_PATH); }
        else if(i == SU_SCRIPT_3)     { strncpy((char*)path, MS_SU_SCRIPT_3, MS_MAX_PATH); }
        else if(i == SU_SCRIPT_4)     { strncpy((char*)path, MS_SU_SCRIPT_4, MS_MAX_PATH); }
        else if(i == SU_SCRIPT_5)     { strncpy((char*)path, MS_SU_SCRIPT_5, MS_MAX_PATH); }
        else if(i == SU_SCRIPT_6)     { strncpy((char*)path, MS_SU_SCRIPT_6, MS_MAX_PATH); }
        else if(i == SU_SCRIPT_7)     { strncpy((char*)path, MS_SU_SCRIPT_7, MS_MAX_PATH); }
        else { return SATR_ERROR; }

        uint8_t fres = i;

        res = f_stat((char*)path, &fno);
        if(res == FR_NO_FILE) { fres = 0; fno.fsize = 0; }
        else if(res != FR_OK) { fres = -1; fno.fsize = 0; } 

        cnv32_8(fres, &buf[(*size)]);
        *size += sizeof(uint32_t);

        cnv32_8(fno.fsize, &buf[(*size)]);
        *size += sizeof(uint32_t);

        cnv16_8(fno.fdate, &buf[(*size)]);
        *size += sizeof(uint16_t);

        cnv16_8(fno.ftime, &buf[(*size)]);
        *size += sizeof(uint16_t);
        
        buf[(*size)] = MNLP_data.su_scripts[(uint8_t) i-1].valid_logi;
        *size += sizeof(uint8_t);
    }
    return SATR_EOT;
}

SAT_returnState mass_storage_su_load_api(MS_sid sid, uint8_t *buf) {

    FIL fp;
    FRESULT res;
    uint8_t path[MS_MAX_PATH];
    uint16_t size = 0;
    uint16_t script_len = 0;

    if(!C_ASSERT(MS_data.enabled == true) == true) { return SATR_SD_DISABLED; }
    if(!C_ASSERT( sid <= SU_SCRIPT_7) == true) { return SATR_INV_STORE_ID; }

    if(sid == SU_SCRIPT_1)          { strncpy((char*)path, MS_SU_SCRIPT_1, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_2)     { strncpy((char*)path, MS_SU_SCRIPT_2, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_3)     { strncpy((char*)path, MS_SU_SCRIPT_3, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_4)     { strncpy((char*)path, MS_SU_SCRIPT_4, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_5)     { strncpy((char*)path, MS_SU_SCRIPT_5, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_6)     { strncpy((char*)path, MS_SU_SCRIPT_6, MS_MAX_PATH); }
    else if(sid == SU_SCRIPT_7)     { strncpy((char*)path, MS_SU_SCRIPT_7, MS_MAX_PATH); }
    else { return SATR_ERROR; }

    if((res = f_open(&fp, (char*)path, FA_OPEN_EXISTING | FA_READ)) != FR_OK) { return res + SATRF_OK; }

    res = f_read(&fp, buf, MS_MAX_SU_FILE_SIZE, (void *)&size);
    f_close(&fp);

    if(res != FR_OK)    { return res + SATRF_OK; }
    else if(size == 0)  { return SATR_ERROR; } 

    union _cnv cnv;
    cnv.cnv8[0] = buf[0];
    cnv.cnv8[1] = buf[1];
    script_len = cnv.cnv16[0];

    if(!C_ASSERT(size == script_len) == true) { return SATR_ERROR; } 

    uint16_t sum1 = 0;
    uint16_t sum2 = 0;

    for(uint16_t i = 0; i < size; i++) {
        sum1 = (sum1 + buf[i]) % 255; 
        sum2 = (sum2 + sum1) % 255;
    }

    if(!C_ASSERT(((sum2 << 8) | sum1) == 0) == true)  { return SATR_CRC_ERROR; }

    return SATR_OK;
}

SAT_returnState mass_storage_init() {

    FRESULT res = 0;

    MS_data.enabled = false;
    if((res = f_mount(&MS_data.Fs, MS_SD_PATH, 0)) != FR_OK) { return res + SATRF_OK; }

    MS_data.enabled = true;

    /*Directory sanity check*/
    if((res = mass_storage_dirCheck()) != SATR_OK)         { return res; }

    return SATR_OK;
}

SAT_returnState mass_storage_updatePkt(tc_tm_pkt *pkt, const uint16_t size, const uint8_t subtype) {

    pkt->ser_subtype = subtype;
    pkt->len = size;

    return SATR_OK;
}

SAT_returnState mass_storage_crtPkt(tc_tm_pkt **pkt, const uint16_t dest_id, const uint16_t size) {

    *pkt = get_pkt(size);
    if(!C_ASSERT(*pkt != NULL) == true) { return SATR_ERROR; }
    crt_pkt(*pkt, OBC_APP_ID, TM, TC_ACK_NO, TC_MASS_STORAGE_SERVICE, 0, dest_id); //what dest_id ?

    return SATR_OK;
}

SAT_returnState mass_storage_FORMAT() {

    FRESULT res;

    for(uint8_t i = 0; i < 5; i++ ) {

        res = f_mount(0, "", 0);

        HAL_Delay(300);

        MS_data.enabled = false;
        power_control_api(OBC_SD_DEV_ID, P_OFF);
        HAL_sys_delay(5000);
        power_control_api(OBC_SD_DEV_ID, P_ON);
        HAL_sys_delay(5000);

        HAL_Delay(300);

        /* Register work area (do not care about error) */
        res = f_mount(&MS_data.Fs, MS_SD_PATH, 0);

        HAL_Delay(100);

        /* Create FAT volume with default cluster size */
        res = f_mkfs(MS_SD_PATH, 0, 0);

        HAL_Delay(300);

        f_mount(0, "", 0);

        power_control_api(OBC_SD_DEV_ID, P_OFF);
        HAL_sys_delay(5000);
        power_control_api(OBC_SD_DEV_ID, P_ON);
        HAL_sys_delay(5000);

        if(res == FR_OK)                                 { break; }
    }

    if(res != FR_OK)                                     { return res + SATRF_OK; }

    res = f_mount(&MS_data.Fs, MS_SD_PATH, 0);

    HAL_Delay(100);

    for(uint8_t i = 0; i < 5; i++ ) {
        if((res = f_mkdir(MS_SU_LOG)) == FR_OK || res == FR_EXIST)          { break; }
        HAL_Delay(1);
    }

    for(uint8_t i = 0; i < 5; i++ ) {
        if((res = f_mkdir(MS_WOD_LOG)) == FR_OK || res == FR_EXIST)         { break; }
        HAL_Delay(1);
    }

    for(uint8_t i = 0; i < 5; i++ ) {
        if((res = f_mkdir(MS_EXT_WOD_LOG)) == FR_OK || res == FR_EXIST)     { break; }
    }

    for(uint8_t i = 0; i < 5; i++ ) {
        if((res = f_mkdir(MS_EVENT_LOG)) == FR_OK || res == FR_EXIST)       { break; }
        HAL_Delay(1);
    }

    for(uint8_t i = 0; i < 5; i++ ) {
        if((res = f_mkdir(MS_FOTOS)) == FR_OK || res == FR_EXIST)           { break; }
        HAL_Delay(1);
    }

    for(uint8_t i = 0; i < 5; i++ ) {
        if((res = f_mkdir(MS_SCHS)) == FR_OK || res == FR_EXIST)            { break; }
        HAL_Delay(1);
    }

    for(uint8_t i = 0; i < 5; i++ ) {
        if((res = f_mkdir("/SU_SCR_1")) == FR_OK || res == FR_EXIST)        { break; }
        HAL_Delay(1);
    }

    for(uint8_t i = 0; i < 5; i++ ) {
        if((res = f_mkdir("/SU_SCR_2")) == FR_OK || res == FR_EXIST)        { break; }
        HAL_Delay(1);
    }

    for(uint8_t i = 0; i < 5; i++ ) {
        if((res = f_mkdir("/SU_SCR_3")) == FR_OK || res == FR_EXIST)        { break; }
        HAL_Delay(1);
    }

    for(uint8_t i = 0; i < 5; i++ ) {
        if((res = f_mkdir("/SU_SCR_4")) == FR_OK || res == FR_EXIST)        { break; }
        HAL_Delay(1);
    }

    for(uint8_t i = 0; i < 5; i++ ) {
        if((res = f_mkdir("/SU_SCR_5")) == FR_OK || res == FR_EXIST)        { break; }
        HAL_Delay(1);
    }

    for(uint8_t i = 0; i < 5; i++ ) {
        if((res = f_mkdir("/SU_SCR_6")) == FR_OK || res == FR_EXIST)        { break; }
        HAL_Delay(1);
    }

    for(uint8_t i = 0; i < 5; i++ ) {
        if((res = f_mkdir("/SU_SCR_7")) == FR_OK || res == FR_EXIST)        { break; }
        HAL_Delay(1);
    }

    MS_data.enabled = true;

    return SATR_OK;
}

SAT_returnState mass_storage_dirCheck() {

    FRESULT res;
    FILINFO fno;

    if((res = f_stat((char*)MS_SU_LOG, &fno)) != FR_OK)         { return res + SATRF_OK; }
    if((res = f_stat((char*)MS_WOD_LOG, &fno)) != FR_OK)        { return res + SATRF_OK; }
    if((res = f_stat((char*)MS_EXT_WOD_LOG, &fno)) != FR_OK)    { return res + SATRF_OK; }
    if((res = f_stat((char*)MS_EVENT_LOG, &fno)) != FR_OK)      { return res + SATRF_OK; }
    if((res = f_stat((char*)MS_FOTOS, &fno)) != FR_OK)          { return res + SATRF_OK; }
    if((res = f_stat((char*)MS_SCHS, &fno)) != FR_OK)           { return res + SATRF_OK; }
    if((res = f_stat("/SU_SCR_1", &fno)) != FR_OK)              { return res + SATRF_OK; }
    if((res = f_stat("/SU_SCR_2", &fno)) != FR_OK)              { return res + SATRF_OK; }
    if((res = f_stat("/SU_SCR_3", &fno)) != FR_OK)              { return res + SATRF_OK; }
    if((res = f_stat("/SU_SCR_4", &fno)) != FR_OK)              { return res + SATRF_OK; }
    if((res = f_stat("/SU_SCR_5", &fno)) != FR_OK)              { return res + SATRF_OK; }
    if((res = f_stat("/SU_SCR_6", &fno)) != FR_OK)              { return res + SATRF_OK; }
    if((res = f_stat("/SU_SCR_7", &fno)) != FR_OK)              { return res + SATRF_OK; }

    return SATR_OK;
}

/**
 * @brief      gets mass storage device state
 *
 * @param      state  true/false
 */
void mass_storage_getState(uint8_t *state) {
    *state = MS_data.enabled;
}
