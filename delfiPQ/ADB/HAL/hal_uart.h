#ifndef __HAL_UART_H
#define __HAL_UART_H

#include <stdint.h>
#include <satellite.h>

//extern UART_Handle uart_dbg_bus;

SAT_returnState HAL_uart_rx(dev_id id, uint8_t *buf, uint16_t *size);

void HAL_uart_tx(dev_id id, uint8_t *buf, uint16_t size);

SAT_returnState HAL_SPI_readWrite(dev_id id,
                                  void *writeBuf,
                                  size_t count,
                                  void *readBuf);

SAT_returnState HAL_I2C_readWrite(dev_id id,
                                  void *writeBuf,
                                  size_t tx_count,
                                  void *readBuf,
                                  size_t rx_count);

#endif
