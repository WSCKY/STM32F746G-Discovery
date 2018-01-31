#ifndef __SYSTEMTIMECOUNTER_H
#define __SYSTEMTIMECOUNTER_H

#include "MCU_TIM_BASE.h"

#define SYSTEM_PRECISE_TIMER_COUNTER()           MCU_TIM3_GetCounter()

uint32_t _Get_Micros(void);
uint32_t _Get_Millis(void);
uint32_t _Get_Seconds(void);

#endif /* __SYSTEMTIMECOUNTER_H */
