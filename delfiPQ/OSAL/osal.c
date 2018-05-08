#include "osal.h"

#include <unistd.h>

void OSAL_sys_delay(uint32_t usec) {
  usleep(usec);
}

uint32_t OSAL_sys_GetTick() {

}

void OSAL_wake_uart_task() {

}
