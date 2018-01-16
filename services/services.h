#ifndef __SERVICES_H
#define __SERVICES_H

/* TM TC services*/
#define ECSS_VER_NUMBER             0
#define ECSS_DATA_FIELD_HDR_FLG     1

#define ECSS_PUS_VER            1
#define ECSS_SEC_HDR_FIELD_FLG  0

/*sequence definitions*/
/*we only support TC_TM_SEQ_SPACKET*/
#define TC_TM_SEQ_FPACKET 0x01
#define TC_TM_SEQ_CPACKET 0x00
#define TC_TM_SEQ_LPACKET 0x02
#define TC_TM_SEQ_SPACKET 0x03

/*services ack req*/
/*should confirm endianess*/
#define TC_ACK_NO           0x00
#define TC_ACK_ACC          0x01
#define TC_ACK_EXE_START    0x02
#define TC_ACK_EXE_PROG     0x04
#define TC_ACK_EXE_COMP     0x08
#define TC_ACK_ALL          0x0F

#define ECSS_HEADER_SIZE        6
#define ECSS_DATA_HEADER_SIZE   4
#define ECSS_CRC_SIZE           2

#define ECSS_DATA_OFFSET        10  /*ECSS_HEADER_SIZE + ECSS_DATA_HEADER_SIZE*/

#define TC 1
#define TM 0

/*services types*/
#define TC_VERIFICATION_SERVICE         1
#define TC_HOUSEKEEPING_SERVICE         3
#define TC_EVENT_SERVICE                5
#define TC_FUNCTION_MANAGEMENT_SERVICE  8
#define TC_TIME_MANAGEMENT_SERVICE      9
#define TC_SCHEDULING_SERVICE           11
#define TC_LARGE_DATA_SERVICE           13
#define TC_MASS_STORAGE_SERVICE         15
#define TC_TEST_SERVICE                 17
#define TC_SU_MNLP_SERVICE              18 /*service number out of ECSS standard, mission specific for mnlp su*/

/*services subtypes*/
#define TM_VR_ACCEPTANCE_SUCCESS        1
#define TM_VR_ACCEPTANCE_FAILURE        2

#define TC_HK_REPORT_PARAMETERS         21
#define TM_HK_PARAMETERS_REPORT         23

#define TM_EV_NORMAL_REPORT         	  1
#define TM_EV_ERROR_REPORT         	    4

#define TC_FM_PERFORM_FUNCTION          1

#define TC_SC_ENABLE_RELEASE            1
#define TC_SC_DISABLE_RELEASE           2
#define TC_SC_RESET_SCHEDULE            3
#define TC_SC_INSERT_TC                 4
#define TC_SC_DELETE_TC                 5
#define TC_SC_TIME_SHIFT_SPECIFIC       7
#define TC_SC_TIME_SHIFT_SELECTED_OTP   8
#define TC_SC_TIME_SHIFT_ALL            15

#define TM_LD_FIRST_DOWNLINK            1
#define TC_LD_FIRST_UPLINK              9
#define TM_LD_INT_DOWNLINK              2
#define TC_LD_INT_UPLINK                10
#define TM_LD_LAST_DOWNLINK             3
#define TC_LD_LAST_UPLINK               11
#define TC_LD_ACK_DOWNLINK              5
#define TM_LD_ACK_UPLINK                14
#define TC_LD_REPEAT_DOWNLINK           6
#define TM_LD_REPEAT_UPLINK             15
#define TM_LD_REPEATED_DOWNLINK         7
#define TC_LD_REPEATED_UPLINK           12
#define TM_LD_ABORT_SE_DOWNLINK         4
#define TC_LD_ABORT_SE_UPLINK           13
#define TC_LD_ABORT_RE_DOWNLINK         8
#define TM_LD_ABORT_RE_UPLINK           16

#define TC_MS_ENABLE                    1
#define TC_MS_DISABLE                   2
#define TM_MS_CONTENT                   8
#define TC_MS_DOWNLINK                  9
#define TC_MS_DELETE                    11
#define TC_MS_REPORT                    12
#define TM_MS_CATALOGUE_REPORT          13
#define TC_MS_UPLINK                    14
#define TC_MS_FORMAT                    15 /*custom service*/
#define TC_MS_LIST                      16 /*custom service*/
#define TM_MS_CATALOGUE_LIST            17

#define TC_CT_PERFORM_TEST              1
#define TM_CT_REPORT_TEST               2

/*mNLP science unit sub-service definitions*/
#define TC_SU_ON                        1  /*subservice 1*/
#define TC_SU_OFF                       2  /*subservice 2*/
#define TC_SU_RESET                     3  /*subservice 3*/
#define TC_SU_LOAD_P                    4  /*subservice 4*/
#define TC_SU_HC                        5  /*subservice 5*/
#define TC_SU_CAL                       6  /*subservice 6*/
#define TC_SU_SCI                       7  /*subservice 7*/
#define TC_SU_HK                        8  /*subservice 8*/
#define TC_SU_STM                       9  /*subservice 9*/
#define TC_SU_DUMP                      10 /*subservice 10*/
#define TC_SU_BIAS_ON                   11 /*subservice 11*/
#define TC_SU_BIAS_OFF                  12 /*subservice 12*/
#define TC_SU_MTEE_ON                   13 /*subservice 13*/
#define TC_SU_MTEE_OFF                  14 /*subservice 14*/
#define TC_SU_SCH_TASK_NTF              15 /*subservice 15*/
#define TC_SU_STATUS_REPORT             16 /*subservice 16*/
#define TM_SU_STATUS_REPORT             17 /*subservice 17*/
#define TC_SU_MANAGE                    18 /*subservice 18*/

/*TIME MANAGEMENT SERVICE*/
#define TM_TIME_SET_IN_UTC              1 /*subservice 1*/
#define TM_TIME_SET_IN_QB50             2 /*subservice 2*/
#define TM_REPORT_TIME_IN_UTC           3 /*subservice 3, Telecommand to report time in UTC*/
#define TM_REPORT_TIME_IN_QB50          4 /*subservice 4, Telecommand to report time in QB50*/
#define TM_TIME_REPORT_IN_UTC           5 /*subservice 5, Telemetry response time in UTC*/
#define TM_TIME_REPORT_IN_QB50          6 /*subservice 6, Telemetry response time in QB50*/

/*SCHEDULING SERVICE*/
#define SCHS_ENABLE_RELEASE             1 /*subservice 01, Telecommand to enable the release of telecommands from schedule pool*/
#define SCHS_DISABLE_RELEASE            2 /*subservice 02, Telecommand to disable the release of telecommands from schedule pool*/
#define SCHS_RESET_SCH                  3 /*subservice 03, Telecommand to reset the schedule pool*/
#define SCHS_INSERT_TC_IN_SCH           4 /*subservice 04, Telecommand to insert a tc_tm_pkt in schedule pool*/
#define SCHS_DELETE_TC_FROM_SCH         5 /*subservice 05, Telecommand to delete a tc_tm_pkt from schedule pool*/
#define SCHS_DELETE_TC_FROM_SCH_OTP     6 /*subservice 06, Telecommand to delete tc_tm_pkts from schedule pool over a time period*/
#define SCHS_TIME_SHIFT_SEL_TC          7 /*subservice 07, Telecommand to time shift (+/-) selected active schedule packet*/
#define SCHS_TIME_SHIFT_SEL_TC_OTP      8 /*subservice 08, Telecommand to time shift (+/-) selected active schedule packets over a time period*/
#define SCHS_DETAILED_SCH_REPORT        10 /*subservice 10, Telemerty response to (to TC no:16) report schedules in detailed form*/
#define SCHS_SIMPLE_SCH_REPORT          13 /*subservice 13, Telemerty response to (to TC no:17) report schedules in summary form*/
#define SCHS_TIME_SHIFT_ALL_TCS         15 /*subservice 15, Telecommand to time shift (+/-) all active schedule packets*/
#define SCHS_REPORT_SCH_DETAILED        16 /*subservice 16, Telecommand to report schedules in detailed form*/
#define SCHS_REPORT_SCH_SUMMARY         17 /*subservice 17, Telecommand to report schedules in summary form*/
#define SCHS_LOAD_SCHEDULES             22 /*subservice 22, Telecommand to load schedules from perm storage*/
#define SCHS_SAVE_SCHEDULES             23 /*subservice 23, Telecommand to save schedules on perm storage*/

#endif
