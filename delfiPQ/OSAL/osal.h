#ifndef __OSAL_H
#define __OSAL_H

#include <stdint.h>
#include "satellite.h"

void OSAL_wake_uart_task();

void OSAL__sys_delay(uint32_t sec);

uint32_t OSAL_sys_GetTick();

#endif
