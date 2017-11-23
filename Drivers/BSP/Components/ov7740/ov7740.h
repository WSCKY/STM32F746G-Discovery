/**
  ******************************************************************************
  * @file    ov7740.h
  * @author  kyChu
  * @version V1.0.0
  * @date    13-November-2017
  * @brief   This file contains all the functions prototypes for the ov7740.c
  *          driver.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OV7740_H
#define __OV7740_H

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

/** @addtogroup OV7740
  * @{
  */

/** @defgroup OV7740_Exported_Types
  * @{
  */

typedef struct
{
	unsigned char Product_ID_H;
	unsigned char Product_ID_L;
	unsigned char Manufacturer_IDH;
	unsigned char Manufacturer_IDL;
} OVCamera_IDTypeDef;

/**
  * @}
  */

/** @defgroup OV7740_Exported_Constants
  * @{
  */

#define OV7740_SLAVE_ADDRESS   0x42

#define OV7740_PIDH_ADDRESS    0x0A
#define OV7740_PIDL_ADDRESS    0x0B
#define OV7740_MIDH_ADDRESS    0x1C
#define OV7740_MIDL_ADDRESS    0x1D

#define OV7740_PIDH            0x77
#define OV7740_PIDL            0x42
#define OV7740_MIDH            0x7F
#define OV7740_MIDL            0xA2

#define OV7740_PID_VER_MASK    0xFFF0

/**
  * @brief  OV7740 Registers
  */

/* OV7740 Registers definition */
#define OV7740_SENSOR_YAVG                0x2F   /* Luminance Average Value, Read Only Register */
#define OV7740_SENSOR_REG63               0x63   /* Luminance Average Value of one zone, This zone was selected by Monitor[3:0] in REG 0x38 */
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
  
/** @defgroup OV7740_Exported_Functions
  * @{
  */ 
void     ov7740_Init(uint16_t DeviceAddr, uint32_t resolution);
void     ov7740_Config(uint16_t DeviceAddr, uint32_t feature, uint32_t value, uint32_t BR_value);
uint16_t ov7740_VerifyID(uint16_t DeviceAddr);

void     CAMERA_IO_Init(void);
void     CAMERA_IO_Write(uint8_t addr, uint8_t reg, uint8_t value);
uint8_t  CAMERA_IO_Read(uint8_t addr, uint8_t reg);
void     CAMERA_Delay(uint32_t delay);

/* CAMERA driver structure */
extern CAMERA_DrvTypeDef ov7740_drv;
/**
  * @}
  */    
#ifdef __cplusplus
}
#endif

#endif /* __OV7740_H */
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
