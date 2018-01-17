

union _cnv {
    double cnvD;
    float cnvF;
    uint32_t cnv32;
    uint16_t cnv16[4];
    uint8_t cnv8[8];
};

extern void HAL_uart_tx(TC_TM_app_id app_id, uint8_t *buf, uint16_t size);
extern SAT_returnState event_log(uint8_t *buf, const uint16_t size);
extern SAT_returnState event_crt_pkt_api(uint8_t *buf, uint8_t *f, uint16_t fi, uint32_t l, uint8_t *e, uint16_t *size, SAT_returnState mode);

#define C_ASSERT(e)    ((e) ? (true) : (tst_debugging(__FILE_ID__, __LINE__, #e)))

/*Lookup table that returns if a service with its subtype with TC or TM is supported and valid*/
extern const uint8_t services_verification_TC_TM[MAX_SERVICES][MAX_SUBTYPES][2];
extern uint8_t assertion_last_file;
extern uint16_t assertion_last_line;

uint8_t tst_debugging(uint16_t fi, uint32_t l, char *e);
