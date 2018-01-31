/**
  ******************************************************************************
  * @file    MCU_TIM_BASE.h
  * @author  kyChu
  * @version V0.1
  * @date    19-January-2018
  * @brief   Header file for MCU Timer Counter Driver.
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MCU_TIM_BASE_H
#define __MCU_TIM_BASE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported definitions ------------------------------------------------------*/
#define TIM3_ENABLE                              (1)
#if (TIM3_ENABLE)
  #define TIM3_UPDATE_IT_ENABLE                  (1)
#endif /* TIM3_ENABLE */
/* Exported macros -----------------------------------------------------------*/
#if (TIM3_ENABLE)
/* Definition for TIM3 peripheral configuration */
  #define TIM3_COUNT_CLOCK_RATE                  (1000000)

  #define TIM3_COUNTER_DIRECTION_DOWN            (0)
  #define TIM3_AUTORELOAD_PRELOAD                (1)
  /* Definition for TIM3 clock resources */
  #define TIM3_CLK_ENABLE()                      __TIM3_CLK_ENABLE()
#endif /* TIM3_ENABLE */
/* Exported functions ------------------------------------------------------- */
HAL_StatusTypeDef MCU_TIMs_Init(void);
#if (TIM3_ENABLE)
uint32_t MCU_TIM3_GetCounter(void);
#endif /* TIM3_ENABLE */

#endif /* __MCU_TIM_BASE_H */

/* ------------------------ (C) COPYRIGHT kyChu ----------- END OF FILE ----- */
