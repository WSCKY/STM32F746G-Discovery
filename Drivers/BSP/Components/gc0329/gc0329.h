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
#define __CAM_MIN(a,b)                 (((a) < (b)) ? (a) : (b))
#define __CAM_MAX(a,b)                 (((a) > (b)) ? (a) : (b))
#define __CAM_LIMIT(x,min,max)         (__CAM_MIN(__CAM_MAX(min, x), max))
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

#define WINDOW_WIDTH                   640
#define WINDOW_HEIGHT                  480

#define IMG_WIDTH                      120
#define IMG_HEIGHT                     120

#define SUB_SAMPLE_WIDTH_MIN           (1)
#define SUB_SAMPLE_HEIGHT_MIN          (1)
#define SUB_SAMPLE_WIDTH_MAX           (WINDOW_WIDTH / IMG_WIDTH)
#define SUB_SAMPLE_HEIGHT_MAX          (WINDOW_HEIGHT / IMG_HEIGHT)

#define SUB_SAMPLE_WIDTH               __CAM_LIMIT(2, SUB_SAMPLE_WIDTH_MIN, SUB_SAMPLE_WIDTH_MAX)
#define SUB_SAMPLE_HEIGHT              __CAM_LIMIT(2, SUB_SAMPLE_HEIGHT_MIN, SUB_SAMPLE_HEIGHT_MAX)

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
