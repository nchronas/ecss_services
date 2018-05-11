#include "hal_uart.h"
#include "packet_engine.h"
#include "OBC_board.h"
#include "satellite.h"
#include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTMSP432.h>
#include <ti/drivers/utils/RingBuf.h>
#include <ti/drivers/I2C.h>

UART_Handle uart_pq9_bus;
UART_Handle uart_dbg_bus;
static I2C_Handle i2c_brd;
static SPI_Handle spi_fram;

void HAL_access_device_peripheral(dev_id id, void ** handle) {

  if(id == OBC_BUS_DEV_ID) {
    *handle = &uart_pq9_bus;
  } else if(id == OBC_DBG_DEV_ID) {
    //*handle = &uart_dbg_bus;
  } else if(id == OBC_MON_DEV_ID ||
            id == OBC_TEMP_DEV_ID ) {
    *handle = &i2c_brd;
  } else if(id == OBC_FRAM_DEV_ID) {
    *handle = &spi_fram;
  }

}

HAL_access_device_peripheral_meta(dev_id id, void *value) {

  if(id == OBC_MON_DEV_ID) {
   *(uint8_t*)value = 0x40;
  } else if(id == OBC_TEMP_DEV_ID) {
    *(uint8_t*)value = 0x48;
  } else if(id == OBC_FRAM_DEV_ID) {
    *(Board_GPIOName*)value = FRAM_CS;
  }

}

void temp(UART_Handle handle, void *buf, size_t count) {
    UARTMSP432_Object *object = handle->object;
    int i = 0;
    i++;
    i = object->readCount;
}

void HAL_peripheral_open() {

  I2C_Params  i2cParams;
  UART_Params uartParams;
  SPI_Params spiParams;

  /* Create a UART with data processing off. */
  UART_Params_init(&uartParams);
  uartParams.writeMode = UART_MODE_BLOCKING;
  uartParams.writeDataMode = UART_DATA_BINARY;
  uartParams.readMode = UART_MODE_CALLBACK;
  uartParams.readDataMode = UART_DATA_BINARY;
  uartParams.readReturnMode = UART_RETURN_FULL;
  uartParams.readEcho = UART_ECHO_OFF;
  uartParams.baudRate = 9600;
  uartParams.readCallback = &temp;
  uart_pq9_bus = UART_open(PQ9, &uartParams);

  UART_Params_init(&uartParams);
  uartParams.writeMode = UART_MODE_BLOCKING;
  uartParams.writeDataMode = UART_DATA_BINARY;
  uartParams.readMode = UART_MODE_BLOCKING;
  uartParams.readDataMode = UART_DATA_BINARY;
  uartParams.readReturnMode = UART_RETURN_FULL;
  uartParams.readEcho = UART_ECHO_ON;
  uartParams.baudRate = 9600;
  uart_dbg_bus = UART_open(DBG, &uartParams);

  I2C_Params_init(&i2cParams);
  i2cParams.transferMode = I2C_MODE_BLOCKING;
  i2cParams.bitRate = I2C_100kHz;
  i2c_brd = I2C_open(I2C_MON, &i2cParams);

  /* Initialize SPI handle as default master */
  SPI_Params_init(&spiParams);
  spiParams.frameFormat = SPI_POL0_PHA0;
  spiParams.bitRate = 100000;
  spi_fram = SPI_open(FRAM, &spiParams);

}

//extern SPI_Handle spi_fram2;

SAT_returnState HAL_SPI_readWrite(dev_id id,
                                  void *writeBuf,
                                  size_t count,
                                  void *readBuf) {

  SPI_Transaction spiTransaction;
  SPI_Handle *spi;
  Board_GPIOName *cs;

  HAL_access_device_peripheral(id, &spi);
  HAL_access_device_peripheral_meta(id, &cs);

  spiTransaction.count = count;
  spiTransaction.txBuf = (void *)writeBuf;
  spiTransaction.rxBuf = (void *)readBuf;

  GPIO_write(FRAM_CS, 0);
  SPI_transfer(*spi, &spiTransaction);
  GPIO_write(FRAM_CS, 1);

  return SATR_OK;
}

SAT_returnState HAL_I2C_readWrite(dev_id id,
                                  void *writeBuf,
                                  size_t tx_count,
                                  void *readBuf,
                                  size_t rx_count) {

  I2C_Transaction i2cTransaction;
  I2C_Handle *i2c;
  uint8_t i2c_add = 0;

  HAL_access_device_peripheral(id, &i2c);
  HAL_access_device_peripheral_meta(id, &i2c_add);

  if(*i2c == NULL) { return SATR_ERROR; }

  i2cTransaction.slaveAddress = i2c_add;
  i2cTransaction.writeBuf = writeBuf;
  i2cTransaction.writeCount = tx_count;
  i2cTransaction.readBuf = readBuf;
  i2cTransaction.readCount = rx_count;
  I2C_transfer((*i2c), &i2cTransaction);

  return SATR_OK;
}

SAT_returnState HAL_uart_rx(dev_id id, uint8_t *buf, uint16_t *size) {

    UARTMSP432_Object *object = uart_pq9_bus->object;

    if(!C_ASSERT(buf != 0) == true) {
      return SATR_ERROR;
    }
    if(!C_ASSERT(object->readCount < UART_BUF_SIZE) == true) {
      return SATR_ERROR;
    }


    if(object->readCount > 0) {
        uint8_t readIn = 0;

        for(uint16_t i = 0; i < object->readCount; i++) {
          uint16_t res = RingBuf_get(&object->ringBuffer, &readIn);

          if(!C_ASSERT(res >= 0) == true) {
            return SATR_ERROR;
          }

          buf[i] = readIn;
        }

        *size = object->readCount;
        object->readCount = 0;

        return SATR_EOT;
    }
    return SATR_OK;
}

void HAL_uart_tx(dev_id id, uint8_t *buf, uint16_t size) {

  if(!C_ASSERT(buf != NULL) == true) {
    return ;
  }
  if(!C_ASSERT(size < MAX_HLDLC_PKT_SIZE) == true) {
    return ;
  }

  UART_write(uart_pq9_bus, buf, size);
}



/*
 *  ======== Modified readIsrBinaryCallback ========
 *  1. Add to ti/drivers/uart/UARTMSP432.c this function
 *  2. add function definition
 *  3. add #define PACKET_STOP_EVENT 0x7C
 *     which is the hldlc stop flag
 *  4. Modify UARTMSP432.c, the staticFxnTable() and
 *     replace readIsrBinaryCallback with
 *     readIsrModBinaryCallback
 *
 *   Build using make in the ti/simplelink_msp432p4_sdk_2_10_00_14/source/ti/drivers
 */

 // #define PACKET_STOP_EVENT 0x7C
 //
 // static bool readIsrModBinaryCallback(UART_Handle handle)
 // {
 //    int                         readIn;
 //    bool                        ret = true;
 //    UARTMSP432_Object          *object = handle->object;
 //    //V1 or without depanding version
 //    UARTMSP432_HWAttrsV1 const *hwAttrs = handle->hwAttrs;
 //
 //    /*
 //     * Here, we will do a quick check if another data byte has come in from
 //     * when the RX interrupt was generated. This helps us avoid (not protect)
 //     * data overruns by processing the next byte without having to wait for
 //     * another RX interrupt ISR to kick in.
 //     */
 //    while (EUSCI_A_CMSIS(hwAttrs->baseAddr)->IFG & EUSCI_A_IFG_RXIFG) {
 //        readIn = MAP_UART_receiveData(hwAttrs->baseAddr);
 //
 //        if (RingBuf_put(&object->ringBuffer, (unsigned char)readIn) == -1) {
 //            DebugP_log1("UART:(%p) Ring buffer full!!", hwAttrs->baseAddr);
 //            ret = false;
 //        }
 //
 //        if(readIn == PACKET_STOP_EVENT) {
 //          object->readCount = RingBuf_getCount(&object->ringBuffer);
 //          object->readCallback(handle,
 //                               object->readBuf,
 //                               object->readCount);
 //        }
 //
 //    }
 //    DebugP_log2("UART:(%p) buffered '0x%02x'", hwAttrs->baseAddr,
 //        (unsigned char) readIn);
 //
 //    return (ret);
 // }
