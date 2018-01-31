/**
  ******************************************************************************
  * @file    Project/Src/MPU6500.h
  * @author  kyChu
  * @version V1.0.0
  * @date    29-January-2018
  * @brief   Header for MPU6500.c module
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MPU6500_H
#define __MPU6500_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor MPU6500_SPI instance used and associated
   resources */
/* Definition for MPU6500_SPI clock resources */
#define MPU6500_SPI                             SPI2
#define MPU6500_SPI_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()
#define MPU6500_SPI_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOI_CLK_ENABLE()
#define MPU6500_SPI_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define MPU6500_SPI_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define MPU6500_SPI_FORCE_RESET()               __HAL_RCC_SPI2_FORCE_RESET()
#define MPU6500_SPI_RELEASE_RESET()             __HAL_RCC_SPI2_RELEASE_RESET()

/* Definition for MPU6500_SPI Pins */
#define MPU6500_SPI_SCK_PIN                     GPIO_PIN_1
#define MPU6500_SPI_SCK_GPIO_PORT               GPIOI
#define MPU6500_SPI_SCK_AF                      GPIO_AF5_SPI2
#define MPU6500_SPI_MISO_PIN                    GPIO_PIN_14
#define MPU6500_SPI_MISO_GPIO_PORT              GPIOB
#define MPU6500_SPI_MISO_AF                     GPIO_AF5_SPI2
#define MPU6500_SPI_MOSI_PIN                    GPIO_PIN_15
#define MPU6500_SPI_MOSI_GPIO_PORT              GPIOB
#define MPU6500_SPI_MOSI_AF                     GPIO_AF5_SPI2

/* Definition for MPU6500_CTRL Pins */
#define MPU6500_SPI_CS_GPIO_PORT                GPIOA
#define MPU6500_SPI_CS_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define MPU6500_SPI_CS_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOA_CLK_DISABLE()
#define MPU6500_SPI_CS_PIN                      GPIO_PIN_8

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int8_t MPU6500_Init(void);
int8_t MPU6500_VerifyID(void);

#endif /* __PMW3901MB_H */

/********************************* END OF FILE ********************************/
