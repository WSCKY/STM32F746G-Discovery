#include "SystemTimeCounter.h"

uint32_t _Get_Micros(void)
{
	return SYSTEM_PRECISE_TIMER_COUNTER();
}

uint32_t _Get_Millis(void)
{
	return (SYSTEM_PRECISE_TIMER_COUNTER() / 1000);
}

uint32_t _Get_Seconds(void)
{
	return (SYSTEM_PRECISE_TIMER_COUNTER() / 1000000);
}
