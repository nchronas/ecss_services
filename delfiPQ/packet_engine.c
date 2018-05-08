#include "packet_engine.h"

#include "packet_services.h"

struct _uart_data {
    uint8_t uart_buf[UART_BUF_SIZE];
    uint8_t uart_ecss_buf[UART_BUF_SIZE];
    uint8_t uart_hdlc_buf[MAX_PKT_SIZE];
} ud;

SAT_returnState import_pkt() {

    SAT_returnState res_uart = SATR_UNINIT;
    SAT_returnState res_deframe = SATR_UNINIT;
    SAT_returnState res_unpack = SATR_UNINIT;

    tc_tm_pkt *pkt;
    uint16_t uart_size = 0;
    uint16_t hldlc_size = 0;

    res_uart = HAL_uart_rx(ud.uart_buf, &uart_size);
    if( res_uart == SATR_EOT ) {
        res_deframe = HLDLC_deframe(ud.uart_buf,
                                    ud.uart_hdlc_buf,
                                    uart_size,
                                    &hldlc_size);
    }
    if(res_deframe == SATR_EOT) {

        pkt = get_pkt(hldlc_size);
        if(!C_ASSERT(pkt != NULL) == true) { return SATR_ERROR; }

        res_unpack = unpack_pkt(&ud.uart_hdlc_buf, pkt, hldlc_size);
    }
    if(res_unpack == SATR_OK) {
        route_pkt(pkt);
    }
    if(res_deframe == SATR_EOT) {
        free_pkt(pkt);
    }

    return SATR_OK;
}

// Add Tx checks
SAT_returnState export_pkt() {

    tc_tm_pkt *pkt = 0;
    uint16_t size = 0;
    SAT_returnState res = SATR_ERROR;

    /* Checks if that the pkt that was transmitted is still in the queue */
    if((pkt = queuePop(1)) ==  NULL) {
      return SATR_OK;
    }

    pack_pkt(ud.uart_ecss_buf,  pkt, &size);

    res = HLDLC_frame(ud.uart_ecss_buf, ud.uart_hdlc_buf, &size);
    if(res == SATR_ERROR) { return SATR_ERROR; }

    if(!C_ASSERT(size > 0) == true) { return SATR_ERROR; }

    HAL_uart_tx( ud.uart_hdlc_buf, size);

    free_pkt(pkt);

    return SATR_OK;
}
