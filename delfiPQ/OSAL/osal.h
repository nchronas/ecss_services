#ifndef __OSAL_H
#define __OSAL_H

#include <stdint.h>
#include "satellite.h"

void OSAL_wake_uart_task();

void OSAL_sys_delay(uint32_t usec);

uint32_t OSAL_sys_GetTick();

#endif
