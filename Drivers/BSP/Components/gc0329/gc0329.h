/**
  ******************************************************************************
  * @file    gc0329.h
  * @author  kyChu
  * @version V1.0.0
  * @date    13-November-2017
  * @brief   This file contains all the functions prototypes for the gc0329.c
  *          driver.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GC0329_H
#define __GC0329_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "../Common/camera.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Components
  * @{
  */

/** @addtogroup GC0329
  * @{
  */

/** @defgroup GC0329_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup GC0329_Exported_Constants
  * @{
  */
/**
  * @brief  GC0329 ID
  */
#define  GC0329_ID         0x62

#define GC0329_DEVICE_ADDR 0x62
/**
  * @brief  GC0329 Registers
  */

/* GC0329 Registers definition */
#define GC0329_SENSOR_ID_REG           0xFB
//#define OV9655_SENSOR_COM7              0x12
//#define OV9655_SENSOR_TSLB              0x3A
//#define OV9655_SENSOR_MTX1              0x4F
//#define OV9655_SENSOR_MTX2              0x50
//#define OV9655_SENSOR_MTX3              0x51
//#define OV9655_SENSOR_MTX4              0x52
//#define OV9655_SENSOR_MTX5              0x53
//#define OV9655_SENSOR_MTX6              0x54
//#define OV9655_SENSOR_BRTN              0x55
//#define OV9655_SENSOR_CNST1             0x56
//#define OV9655_SENSOR_CNST2             0x57

/** 
 * @brief  OV9655 Features Parameters  
 */
//#define OV9655_BRIGHTNESS_LEVEL0        0xB0     /* Brightness level -2         */
//#define OV9655_BRIGHTNESS_LEVEL1        0x98     /* Brightness level -1         */
//#define OV9655_BRIGHTNESS_LEVEL2        0x00     /* Brightness level 0          */
//#define OV9655_BRIGHTNESS_LEVEL3        0x18     /* Brightness level +1         */
//#define OV9655_BRIGHTNESS_LEVEL4        0x30     /* Brightness level +2         */

//#define OV9655_BLACK_WHITE_BW           0xCC000000000000  /* Black and white effect      */
//#define OV9655_BLACK_WHITE_NEGATIVE     0xEC808000008080  /* Negative effect             */
//#define OV9655_BLACK_WHITE_BW_NEGATIVE  0xEC000000000000  /* BW and Negative effect      */
//#define OV9655_BLACK_WHITE_NORMAL       0xCC808000008080  /* Normal effect               */

//#define OV9655_CONTRAST_LEVEL0          0x30     /* Contrast level -2           */
//#define OV9655_CONTRAST_LEVEL1          0x38     /* Contrast level -1           */
//#define OV9655_CONTRAST_LEVEL2          0x40     /* Contrast level 0            */
//#define OV9655_CONTRAST_LEVEL3          0x50     /* Contrast level +1           */
//#define OV9655_CONTRAST_LEVEL4          0x60     /* Contrast level +2           */

//#define OV9655_COLOR_EFFECT_NONE        0xCC808000008080  /* No color effect             */
//#define OV9655_COLOR_EFFECT_ANTIQUE     0xCC000020F00000  /* Antique effect              */
//#define OV9655_COLOR_EFFECT_BLUE        0xCC000000000060  /* Blue effect                 */
//#define OV9655_COLOR_EFFECT_GREEN       0xCC000000008000  /* Green effect                */
//#define OV9655_COLOR_EFFECT_RED         0xCC600000000000  /* Red effect                  */
/**
  * @}
  */
  
/** @defgroup GC0329_Exported_Functions
  * @{
  */ 
void     gc0329_Init(uint16_t DeviceAddr, uint32_t resolution);
void     gc0329_Config(uint16_t DeviceAddr, uint32_t feature, uint32_t value, uint32_t BR_value);
uint16_t gc0329_ReadID(uint16_t DeviceAddr);

void     CAMERA_IO_Init(void);
void     CAMERA_IO_Write(uint8_t addr, uint8_t reg, uint8_t value);
uint8_t  CAMERA_IO_Read(uint8_t addr, uint8_t reg);
void     CAMERA_Delay(uint32_t delay);

/* CAMERA driver structure */
extern CAMERA_DrvTypeDef gc0329_drv;
/**
  * @}
  */    
#ifdef __cplusplus
}
#endif

#endif /* __GC0329_H */
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
