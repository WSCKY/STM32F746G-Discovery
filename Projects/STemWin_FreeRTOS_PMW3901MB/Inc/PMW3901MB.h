/**
  ******************************************************************************
  * @file    Project/Src/PMW3901MB.h
  * @author  kyChu
  * @version V1.0.0
  * @date    15-December-2017
  * @brief   Header for PMW3901MB.c module
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PMW3901MB_H
#define __PMW3901MB_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Exported types ------------------------------------------------------------*/
typedef struct {
	uint8_t Delta_X_L;
	uint8_t Delta_X_H;
	uint8_t Delta_Y_L;
	uint8_t Delta_Y_H;
} PMW3901MB_DeltaDataDef;
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor PMW3901_SPI instance used and associated
   resources */
/* Definition for PMW3901_SPI clock resources */
#define PMW3901_SPI                             SPI2
#define PMW3901_SPI_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()
#define PMW3901_SPI_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOI_CLK_ENABLE()
#define PMW3901_SPI_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define PMW3901_SPI_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define PMW3901_SPI_FORCE_RESET()               __HAL_RCC_SPI2_FORCE_RESET()
#define PMW3901_SPI_RELEASE_RESET()             __HAL_RCC_SPI2_RELEASE_RESET()

/* Definition for PMW3901_SPI Pins */
#define PMW3901_SPI_SCK_PIN                     GPIO_PIN_1
#define PMW3901_SPI_SCK_GPIO_PORT               GPIOI
#define PMW3901_SPI_SCK_AF                      GPIO_AF5_SPI2
#define PMW3901_SPI_MISO_PIN                    GPIO_PIN_14
#define PMW3901_SPI_MISO_GPIO_PORT              GPIOB
#define PMW3901_SPI_MISO_AF                     GPIO_AF5_SPI2
#define PMW3901_SPI_MOSI_PIN                    GPIO_PIN_15
#define PMW3901_SPI_MOSI_GPIO_PORT              GPIOB
#define PMW3901_SPI_MOSI_AF                     GPIO_AF5_SPI2

/* Definition for PMW3901_CTRL Pins */
#define PMW3901_SPI_CS_GPIO_PORT                GPIOA
#define PMW3901_SPI_CS_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define PMW3901_SPI_CS_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOA_CLK_DISABLE()
#define PMW3901_SPI_CS_PIN                      GPIO_PIN_8

#define PMW3901_NRST_GPIO_PORT                  GPIOA
#define PMW3901_NRST_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOA_CLK_ENABLE()
#define PMW3901_NRST_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOA_CLK_DISABLE()
#define PMW3901_NRST_PIN                        GPIO_PIN_15

#define PMW3901_MOTION_GPIO_PORT                GPIOI
#define PMW3901_MOTION_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOI_CLK_ENABLE()
#define PMW3901_MOTION_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOI_CLK_DISABLE()
#define PMW3901_MOTION_PIN                      GPIO_PIN_0

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int8_t PMW3901_Init(void);
int8_t PMW3901_VerifyID(void);
PMW3901MB_DeltaDataDef *ReadDeltaDataRaw(void);

#endif /* __PMW3901MB_H */

/********************************* END OF FILE ********************************/
