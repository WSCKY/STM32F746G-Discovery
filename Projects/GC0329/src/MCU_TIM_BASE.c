/**
  ******************************************************************************
  * @file    MCU_TIM_BASE.c
  * @author  kyChu
  * @version V0.1
  * @date    19-January-2018
  * @brief   MCU Timer Counter Driver.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "MCU_TIM_BASE.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if (TIM3_ENABLE)
#if (TIM3_UPDATE_IT_ENABLE)
static uint32_t TIM3_CNT_EXTEND = 0;
#endif /* TIM3_UPDATE_IT_ENABLE */
#endif /* TIM3_ENABLE */
/* Private function prototypes -----------------------------------------------*/
#if (TIM3_ENABLE)
  static HAL_StatusTypeDef TIM3_PeriphInit(void);
#endif /* TIM3_ENABLE */
/* Private functions ---------------------------------------------------------*/

/*
 * @brief  Initializes MCU TIMs.
 * @param  None
 * @retval HAL Status.
 */
HAL_StatusTypeDef MCU_TIMs_Init(void)
{
	HAL_StatusTypeDef ret = HAL_OK;
#if (TIM3_ENABLE)
	ret = TIM3_PeriphInit(); if(ret != HAL_OK) return ret;
#endif /* TIM3_ENABLE */
	return ret;
}

#if (TIM3_ENABLE)
/*
 * @brief  Get TIM3 Counter Value.
 * @param  None
 * @retval Counter Value.
 */
uint32_t MCU_TIM3_GetCounter(void)
{
#if (TIM3_UPDATE_IT_ENABLE)
	uint32_t T1, T2;
	do {
		T1 = (TIM3_CNT_EXTEND << 16) | TIM3->CNT;
		T2 = (TIM3_CNT_EXTEND << 16) | TIM3->CNT;
	} while(T1 != T2);
	return T1;
#else
	return TIM3->CNT;
#endif /* TIM3_UPDATE_IT_ENABLE */
}
#endif /* TIM3_ENABLE */

#if (TIM3_ENABLE)
/*
 * @brief  MCU TIM3 Peripheral Initialize.
 * @param  None
 * @retval None
 */
static HAL_StatusTypeDef TIM3_PeriphInit(void)
{
	HAL_StatusTypeDef ret = HAL_OK;
	uint32_t uhPrescalerValue = 0;
	/* Compute the prescaler value to have TIM5 counter clock equal to TIM5_COUNT_CLOCK_RATE Hz */
	uhPrescalerValue = (uint32_t)((SystemCoreClock /2) / TIM3_COUNT_CLOCK_RATE) - 1;

	/* TIM3 Peripheral clock enable */
	TIM3_CLK_ENABLE();

	TIM3->PSC = uhPrescalerValue;
	TIM3->ARR = 0xFFFF;

#if (TIM3_UPDATE_IT_ENABLE)
	TIM3->DIER |= 0x0001;
	/* Set the TIM3 priority */
	HAL_NVIC_SetPriority(TIM3_IRQn, 3, 0);
	/* Enable the TIM3 global Interrupt */
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
#endif /* TIM3_UPDATE_IT_ENABLE */

	TIM3->CNT = 0;
	TIM3->EGR = 0x0001;
	TIM3->CR1 = (TIM3_AUTORELOAD_PRELOAD << 7) | (TIM3_COUNTER_DIRECTION_DOWN << 4) | 0x00000001;

	return ret;
}
#endif /* TIM3_ENABLE */

#if (TIM3_ENABLE)
#if (TIM3_UPDATE_IT_ENABLE)
/*
 * @brief  This function handles TIM3 interrupt request.
 * @param  None
 * @retval None
 */
void TIM3_IRQHandler(void)
{
	if((TIM3->SR & 0x0001) == 0x0001) {
		TIM3->SR &= 0xFFFE;
		TIM3_CNT_EXTEND ++;
	}
}
#endif /* TIM3_UPDATE_IT_ENABLE */
#endif /* TIM3_ENABLE */
