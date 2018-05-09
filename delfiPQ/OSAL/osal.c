#include "osal.h"

#include <unistd.h>
#include <xdc/runtime/Timestamp.h>

void OSAL_sys_delay(uint32_t usec) {
  usleep(usec);
}

uint32_t OSAL_sys_GetTick() {
    return Timestamp_get32();
}

void OSAL_wake_uart_task() {

}
