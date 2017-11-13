/**
  ******************************************************************************
  * @file    gc0329.c
  * @author  kyChu
  * @version V1.0.0
  * @date    13-November-2017
  * @brief   This file provides the GC0329 camera driver
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gc0329.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup GC0329
  * @brief     This file provides a set of functions needed to drive the
  *            GC0329 Camera module.
  * @{
  */

/** @defgroup GC0329_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @defgroup GC0329_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup GC0329_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup GC0329_Private_FunctionPrototypes
  * @{
  */
//static uint64_t gc0329_ConvertValue(uint32_t feature, uint32_t value);
/**
  * @}
  */

/** @defgroup GC0329_Private_Variables
  * @{
  */

CAMERA_DrvTypeDef gc0329_drv =
{
  gc0329_Init,
  gc0329_ReadID,
  gc0329_Config,
};

/* Initialization sequence for GC0329 */
const unsigned char GC0329_CFG[][2] = {
  {0xfe, 0x80}, //soft_reset && page select - 0
  {0xfc, 0x16}, //digital clock enable && da25_en && da18_en
  {0xfc, 0x16}, //digital clock enable && da25_en && da18_en
	/*---------- Page 0 ----------*/
  {0xfe, 0x00}, //page select - 0
  {0x70, 0x40}, //global gain, 2.6 bits
  {0x73, 0x80}, //R channel pre gain, 1.7 bits
  {0x74, 0x80}, //G1 channel pre gain, 1.7 bits
  {0x75, 0x80}, //G2 channel pre gain, 1.7 bits
  {0x76, 0x80}, //B channel pre gain, 1.7 bits //80 jambo
  {0x77, 0x50}, //red channel gain from auto white balancing, 2.6 bits
  {0x78, 0x40}, //green channel gain from auto white balancing, 2.6 bits
  {0x79, 0x48}, //blue channel gain from auto white balancing, 2.6 bits
  {0x03, 0x02},
  {0x04, 0x40},
////////////////////analog////////////////////
  {0xfc, 0x16}, //digital clock enable && da25_en && da18_en
  {0x09, 0x00}, //defines the starting row of the pixel array
  {0x0a, 0x02},
  {0x0b, 0x00}, //defines the starting column of the pixel array
  {0x0c, 0x02},
	{0x0d, 0x01}, //window height high bit  0x01
	{0x0e, 0x18}, //window height low 8 bit 0xe8
	{0x0f, 0x01}, //window width high bit   0x02
	{0x10, 0xe8}, //window width low bit    0x88
  {0x17, 0x00}, //0x14                                                     /**********/
  {0x19, 0x05}, //Reserved
//  {0x1b, 0x24},
//  {0x1c, 0x04},
//  {0x1e, 0x08},
//  {0x1f, 0x08}, //Reserved //C8
//  {0x20, 0x00}, // 0x01
//  {0x21, 0x40}, //Reserved //0x48
//  {0x22, 0xba},
//  {0x23, 0x22},
//  {0x24, 0x16},
////////////////////blk////////////////////
//  {0x26, 0xf7}, //BLK
//  {0x28, 0x7f}, //BLK limit
  {0x29, 0x00},
//  {0x32, 0x00}, //04 darkc
//  {0x33, 0x20}, //blk ratio
//  {0x34, 0x20},
//  {0x35, 0x20},
//  {0x36, 0x20},

//  {0x3b, 0x04}, //manual oset
//  {0x3c, 0x04},
//  {0x3d, 0x04},
//  {0x3e, 0x04},
/////////////},///////ISP BLOCK ENABLE////////////////////
  {0x40, 0xff},
  {0x41, 0x24},//[5]skin detection
  {0x42, 0xfa},//disable ABS
  {0x46, 0x02},
  {0x4b, 0xca},
  {0x4d, 0x01},
//  {0x4f, 0x01},
  {0x70, 0x40}, //global gain, 2.6 bits
  {0x55, 0x01}, //out window height[8]
  {0x56, 0x10}, //out window height[7:0]
	{0x57, 0x01}, //out window width[9:8]
  {0x58, 0xe0}, //out window width[7:0]

////////////////////DNDD////////////////////
//  {0x80, 0x07}, // 0xe7 20140915
//  {0x81, 0xc2}, // 0x22 20140915
//  {0x82, 0x90}, //DN auto DNDD DEC DNDD //0e //55 jambo
  {0x83, 0x05},
//  {0x87, 0x40}, //0x4a  20140915
////////////////////INTPEE////////////////////
//  {0x90, 0x8c}, //ac
  {0x92, 0x05},
  {0x94, 0x05},
  {0x95, 0x45}, //0x44
  {0x96, 0x88},
////////////////////ASDE////////////////////
//	/*---------- Page 1 ----------*/
//  {0xfe, 0x01}, //page select - 1
//  {0x18, 0x22},
	/*---------- Page 0 ----------*/
  {0xfe, 0x00}, //page select - 0
  {0x9c, 0x0a},
  {0xa0, 0xaf},
  {0xa2, 0xff},
  {0xa4, 0x30}, //50 jambo
  {0xa5, 0x31},
  {0xa7, 0x35},
////////////////////RGB gamma////////////////////
//	/*---------- Page 0 ----------*/
//  {0xfe, 0x00}, //page select - 0
  {0xbf, 0x0b},
  {0xc0, 0x1d},
  {0xc1, 0x33},
  {0xc2, 0x49},
  {0xc3, 0x5d},
  {0xc4, 0x6e},
  {0xc5, 0x7c},
  {0xc6, 0x99},
  {0xc7, 0xaf},
  {0xc8, 0xc2},
  {0xc9, 0xd0},
  {0xca, 0xda},
  {0xcb, 0xe2},
  {0xcc, 0xe7},
  {0xcd, 0xf0},
  {0xce, 0xf7},
  {0xcf, 0xff},
////////////////////Y gamma////////////////////
//	/*---------- Page 0 ----------*/
//  {0xfe, 0x00}, //page select - 0
  {0x63, 0x00},
  {0x64, 0x06},
  {0x65, 0x0d},
  {0x66, 0x1b},
  {0x67, 0x2b},
  {0x68, 0x3d},
  {0x69, 0x50},
  {0x6a, 0x60},
  {0x6b, 0x80},
  {0x6c, 0xa0},
  {0x6d, 0xc0},
  {0x6e, 0xe0},
  {0x6f, 0xff},
//////////////////CC///////////////////
//	/*---------- Page 0 ----------*/
//  {0xfe, 0x00}, //page select - 0
  {0xb3, 0x44},
  {0xb4, 0xfd},
  {0xb5, 0x02},
  {0xb6, 0xfa},
  {0xb7, 0x48},
  {0xb8, 0xf0},
// crop
  {0x50, 0x01},
////////////////////YCP////////////////////
//	/*---------- Page 0 ----------*/
//  {0xfe, 0x00}, //page select - 0

  {0xd0, 0x40},
  {0xd1, 0x28},
  {0xd2, 0x28},

  {0xd3, 0x40}, //cont 0x40
  {0xd5, 0x00},

  {0xdd, 0x14},
  {0xde, 0x34},
////////////////////AEC////////////////////
	/*---------- Page 1 ----------*/
  {0xfe, 0x01}, //page select - 1
  {0x10, 0x40}, // before Gamma 
  {0x11, 0x21}, //
  {0x12, 0x13},	// center weight *2
  {0x13, 0x50}, //4 //4 // AE Target
  {0x17, 0xa8},	//88, 08, c8, a8
  {0x1a, 0x21},
  {0x20, 0x31},	//AEC stop margin
  {0x21, 0xc0},
  {0x22, 0x60},
  {0x3c, 0x50},
  {0x3d, 0x40},
  {0x3e, 0x45}, //read 3f for status
////////////////////AWB////////////////////
//	/*---------- Page 1 ----------*/
//  {0xfe, 0x01}, //page select - 1
  {0x06, 0x12},
  {0x07, 0x06},
  {0x08, 0x9c},
  {0x09, 0xee},
  {0x50, 0xfc},
  {0x51, 0x28},
  {0x52, 0x10},
  {0x53, 0x20},
  {0x54, 0x12},
  {0x55, 0x16},
  {0x56, 0x30},
	{0x57, 0x02},
  {0x58, 0x60},
  {0x59, 0x08},
  {0x5a, 0x02},
  {0x5b, 0x63},
  {0x5c, 0x35},
  {0x5d, 0x72},
  {0x5e, 0x11},
  {0x5f, 0x40},
  {0x60, 0x40},
  {0x61, 0xc8},
  {0x62, 0xa0},
  {0x63, 0x40},
  {0x64, 0x50},
  {0x65, 0x98},
  {0x66, 0xfa},
  {0x67, 0x80},
  {0x68, 0x60},
  {0x69, 0x90},
  {0x6a, 0x40},
  {0x6b, 0x39},
  {0x6c, 0x30},
  {0x6d, 0x60},
  {0x6e, 0x41},
  {0x70, 0x10},
  {0x71, 0x00},
  {0x72, 0x10},
  {0x73, 0x40},
  {0x80, 0x60},
  {0x81, 0x50},
  {0x82, 0x42},
  {0x83, 0x40},
  {0x84, 0x40},
  {0x85, 0x40},
  {0x74, 0x40},
  {0x75, 0x58},
  {0x76, 0x24},
  {0x77, 0x40},
  {0x78, 0x20},
  {0x79, 0x60},
  {0x7a, 0x58},
  {0x7b, 0x20},
  {0x7c, 0x30},
  {0x7d, 0x35},
  {0x7e, 0x10},
  {0x7f, 0x08},
///////////////////ABS///////////////////////
  {0x9c, 0x00},
  {0x9e, 0xc0},
  {0x9f, 0x40},
////////////////////CC-AWB////////////////////
  {0xd0, 0x00},
  {0xd2, 0x2c},
  {0xd3, 0x80},
///////////////////LSC //////////////////////
//	/*---------- Page 1 ----------*/
//  {0xfe, 0x01}, //page select - 1
  {0xc0, 0x0b},
  {0xc1, 0x07},
  {0xc2, 0x05},
  {0xc6, 0x0b},
  {0xc7, 0x07},
  {0xc8, 0x05},
  {0xba, 0x39},
  {0xbb, 0x24},
  {0xbc, 0x23},
  {0xb4, 0x39},
  {0xb5, 0x24},
  {0xb6, 0x23},
  {0xc3, 0x00},
  {0xc4, 0x00},
  {0xc5, 0x00},
  {0xc9, 0x00},
  {0xca, 0x00},
  {0xcb, 0x00},
  {0xbd, 0x2b},
  {0xbe, 0x00},
  {0xbf, 0x00},
  {0xb7, 0x09},
  {0xb8, 0x00},
  {0xb9, 0x00},
  {0xa8, 0x31},
  {0xa9, 0x23},
  {0xaa, 0x20},
  {0xab, 0x31},
  {0xac, 0x23},
  {0xad, 0x20},
  {0xae, 0x31},
  {0xaf, 0x23},
  {0xb0, 0x20},
  {0xb1, 0x31},
  {0xb2, 0x23},
  {0xb3, 0x20},
  {0xa4, 0x00},
  {0xa5, 0x00},
  {0xa6, 0x00},
  {0xa7, 0x00},
  {0xa1, 0x3c},
  {0xa2, 0x50},
	/*---------- Page 0 ----------*/
  {0xfe, 0x00}, //page select - 0
//////////////////// flicker ///////////////////
  {0x05, 0x02},
  {0x06, 0x2c},
  {0x07, 0x00},
  {0x08, 0xb8},
	/*---------- Page 1 ----------*/
  {0xfe, 0x01}, //page select - 1
  {0x29, 0x00}, //anti-flicker step [11:8]
  {0x2a, 0x60}, //anti-flicker step [7:0]
  {0x2b, 0x02}, //exp level 0  14.28fps
  {0x2c, 0xa0},
  {0x2d, 0x03}, //exp level 1  12.50fps
  {0x2e, 0x00},
  {0x2f, 0x03}, //exp level 2  10.00fps
  {0x30, 0xc0},
  {0x31, 0x05}, //exp level 3  7.14fps
  {0x32, 0x40},
	/*---------- Page 0 ----------*/
  {0xfe, 0x00}, //page select - 0
////////////////////out ///////////////////
//  {0x44, 0xa2},
	{0x43, 0x00},
	{0x44, 0xa6}, //output format: RGB565
	{0x4e, 0x09},
  {0xf0, 0x07}, //pclk_en && hsync_en && vsync_en
  {0xf1, 0x01}, //normal data output enable
};

/**
  * @}
  */
  
/** @defgroup GC0329_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the GC0329 CAMERA component.
  * @param  DeviceAddr: Device address on communication Bus.
  * @param  resolution: Camera resolution
  * @retval None
  */
void gc0329_Init(uint16_t DeviceAddr, uint32_t resolution)
{
	uint32_t index;

	/* Initialize I2C */
	CAMERA_IO_Init();

	/* Prepare the camera to be configured by resetting all its registers */
//	CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_COM7, 0x80);
	CAMERA_Delay(200);

	/* Initialize GC0329 */
	for(index=0; index<(sizeof(GC0329_CFG)/2); index++)
	{
		CAMERA_IO_Write(DeviceAddr, GC0329_CFG[index][0], GC0329_CFG[index][1]);
		CAMERA_Delay(2);
	}
}

/**
  * @brief  Configures the GC0329 camera feature.
  * @param  DeviceAddr: Device address on communication Bus.
  * @param  feature: Camera feature to be configured
  * @param  value: Value to be configured
  * @param  brightness_value: Brightness value to be configured
  * @retval None
  */
void gc0329_Config(uint16_t DeviceAddr, uint32_t feature, uint32_t value, uint32_t brightness_value)
{
//  uint8_t tslb, mtx1, mtx2, mtx3, mtx4, mtx5, mtx6;
//  uint64_t value_tmp;
//  uint32_t br_value;

//  /* Convert the input value into GC0329 parameters */
//  value_tmp = gc0329_ConvertValue(feature, value);
//  br_value = (uint32_t)gc0329_ConvertValue(CAMERA_CONTRAST_BRIGHTNESS, brightness_value);

//  switch(feature)
//  {
//  case CAMERA_CONTRAST_BRIGHTNESS:
//    {
//      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_BRTN, br_value);
//      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_CNST1, value_tmp);
//      break;
//    }
//  case CAMERA_BLACK_WHITE:
//  case CAMERA_COLOR_EFFECT:
//    {
//      tslb = (uint8_t)(value_tmp >> 48);
//      mtx1 = (uint8_t)(value_tmp >> 40);
//      mtx2 = (uint8_t)(value_tmp >> 32);
//      mtx3 = (uint8_t)(value_tmp >> 24);
//      mtx4 = (uint8_t)(value_tmp >> 16);
//      mtx5 = (uint8_t)(value_tmp >> 8);
//      mtx6 = (uint8_t)(value_tmp);
//      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_TSLB, tslb);
//      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_MTX1, mtx1);
//      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_MTX2, mtx2);
//      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_MTX3, mtx3);
//      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_MTX4, mtx4);
//      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_MTX5, mtx5);
//      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_MTX6, mtx6);
//      break;
//    }
//  default:
//    {
//      break;
//    }
//  }
}

/**
  * @brief  Read the GC0329 Camera identity.
  * @param  DeviceAddr: Device address on communication Bus.
  * @retval the GC0329 ID
  */
uint16_t gc0329_ReadID(uint16_t DeviceAddr)
{
  /* Initialize I2C */
  CAMERA_IO_Init();

  /* Get the camera ID */
  return (CAMERA_IO_Read(DeviceAddr, GC0329_SENSOR_ID_REG));
}

/******************************************************************************
                            Static Functions
*******************************************************************************/
/**
  * @brief  Convert input values into GC0329 parameters.
  * @param  feature: Camera feature to be configured
  * @param  value: Value to be configured
  * @retval The converted value
  */
//static uint64_t gc0329_ConvertValue(uint32_t feature, uint32_t value)
//{
//  uint64_t ret = 0;

//  switch(feature)
//  {
//  case CAMERA_BLACK_WHITE:
//    {
//      switch(value)
//      {
//      case CAMERA_BLACK_WHITE_BW:
//        {
//          ret =  OV9655_BLACK_WHITE_BW;
//          break;
//        }
//      case CAMERA_BLACK_WHITE_NEGATIVE:
//        {
//          ret =  OV9655_BLACK_WHITE_NEGATIVE;
//          break;
//        }
//      case CAMERA_BLACK_WHITE_BW_NEGATIVE:
//        {
//          ret =  OV9655_BLACK_WHITE_BW_NEGATIVE;
//          break;
//        }
//      case CAMERA_BLACK_WHITE_NORMAL:
//        {
//          ret =  OV9655_BLACK_WHITE_NORMAL;
//          break;
//        }
//      default:
//        {
//          ret =  OV9655_BLACK_WHITE_NORMAL;
//          break;
//        }
//      }
//      break;
//    }
//  case CAMERA_CONTRAST_BRIGHTNESS:
//    {
//      switch(value)
//      {
//      case CAMERA_BRIGHTNESS_LEVEL0:
//        {
//          ret =  OV9655_BRIGHTNESS_LEVEL0;
//          break;
//        }
//      case CAMERA_BRIGHTNESS_LEVEL1:
//        {
//          ret =  OV9655_BRIGHTNESS_LEVEL1;
//          break;
//        }
//      case CAMERA_BRIGHTNESS_LEVEL2:
//        {
//          ret =  OV9655_BRIGHTNESS_LEVEL2;
//          break;
//        }
//      case CAMERA_BRIGHTNESS_LEVEL3:
//        {
//          ret =  OV9655_BRIGHTNESS_LEVEL3;
//          break;
//        }
//      case CAMERA_BRIGHTNESS_LEVEL4:
//        {
//          ret =  OV9655_BRIGHTNESS_LEVEL4;
//          break;
//        }
//      case CAMERA_CONTRAST_LEVEL0:
//        {
//          ret =  OV9655_CONTRAST_LEVEL0;
//          break;
//        }
//      case CAMERA_CONTRAST_LEVEL1:
//        {
//          ret =  OV9655_CONTRAST_LEVEL1;
//          break;
//        }
//      case CAMERA_CONTRAST_LEVEL2:
//        {
//          ret =  OV9655_CONTRAST_LEVEL2;
//          break;
//        }
//      case CAMERA_CONTRAST_LEVEL3:
//        {
//          ret =  OV9655_CONTRAST_LEVEL3;
//          break;
//        }
//      case CAMERA_CONTRAST_LEVEL4:
//        {
//          ret =  OV9655_CONTRAST_LEVEL4;
//          break;
//        }
//      default:
//        {
//          ret =  OV9655_CONTRAST_LEVEL0;
//          break;
//        }
//      }
//      break;
//    }
//  case CAMERA_COLOR_EFFECT:
//    {
//      switch(value)
//      {
//      case CAMERA_COLOR_EFFECT_ANTIQUE:
//        {
//          ret =  OV9655_COLOR_EFFECT_ANTIQUE;
//          break;
//        }
//      case CAMERA_COLOR_EFFECT_BLUE:
//        {
//          ret =  OV9655_COLOR_EFFECT_BLUE;
//          break;
//        }
//      case CAMERA_COLOR_EFFECT_GREEN:
//        {
//          ret =  OV9655_COLOR_EFFECT_GREEN;
//          break;
//        }
//      case CAMERA_COLOR_EFFECT_RED:
//        {
//          ret =  OV9655_COLOR_EFFECT_RED;
//          break;
//        }
//      case CAMERA_COLOR_EFFECT_NONE:
//      default:
//        {
//          ret =  OV9655_COLOR_EFFECT_NONE;
//          break;
//        }
//      }
//      break;
//    default:
//      {
//        ret = 0;
//        break;
//      }
//    }
//  }

//  return ret;
//}

/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */

/******************************** END OF FILE *********************************/
