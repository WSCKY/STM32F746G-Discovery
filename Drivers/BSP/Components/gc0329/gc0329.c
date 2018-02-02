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

#define OUTPUT_FORMAT_RGB565           0x06
#define OUTPUT_FORMAT_ONLY_Y           0x11
#define OUTPUT_FORMAT_ONLY_R           0x14
#define OUTPUT_FORMAT_ONLY_G           0x15
#define OUTPUT_FORMAT_ONLY_B           0x16

#define IMG_FORMAT                     OUTPUT_FORMAT_ONLY_Y

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
  {0xfe, 0xd0}, //soft_reset && page select - 0
////////////////////analog////////////////////
  {0xfc, 0x16}, //digital clock enable && da25_en && da18_en
  {0xfc, 0x16}, //digital clock enable && da25_en && da18_en
	{0xfa, 0x00}, //frequency division number
	/*---------- Page 0 ----------*/
  {0xfe, 0x00}, //page select - 0

	{0x4f, 0x01}, //[0] AEC enable
	{0x70, 0x40}, //global gain, 2.6 bits
	{0x71, 0x40}, //Controlled by AEC, can be manually controlled when disable AEC
	{0x72, 0x40}, //Controlled by AEC, can be manually controlled when disable AEC
  {0x03, 0x00}, //Exposure[11:8], use line processing time as unit.
  {0x04, 0x66}, //Exposure[7:0], controlled by AEC if AEC is in function
	{0x73, 0x80}, //R channel pre gain, 1.7 bits
  {0x74, 0x80}, //G1 channel pre gain, 1.7 bits
  {0x75, 0x80}, //G2 channel pre gain, 1.7 bits
  {0x76, 0x80}, //B channel pre gain, 1.7 bits
  {0x77, 0x50}, //red channel gain from auto white balancing, 2.6 bits
  {0x78, 0x40}, //green channel gain from auto white balancing, 2.6 bits
  {0x79, 0x48}, //blue channel gain from auto white balancing, 2.6 bits
	/* ---------- Timing ---------- */
  {0x05, 0x00}, //Horizintal blanking, unit pixel clock, default 0x00.
  {0x06, 0x6a}, //Horizintal blanking, unit pixel clock, default 0x6a.
  {0x07, 0x00}, //Vertical blanking, default 0x00.
  {0x08, 0x70}, //Vertical blanking, default 0x70.
	{0x09, (ROW_START_OFFSET >> 8) & 0x01}, //defines the starting row of the pixel array
  {0x0a, ROW_START_OFFSET & 0xFF},
  {0x0b, (COL_START_OFFSET >> 8) & 0x03}, //defines the starting column of the pixel array
  {0x0c, COL_START_OFFSET & 0xFF},
	{0x0d, ((WINDOW_HEIGHT + 8) >> 8) & 0x01}, //window height high 1 bit
	{0x0e, (WINDOW_HEIGHT + 8) & 0xFF}, //window height low 8 bit
	{0x0f, ((WINDOW_WIDTH + 8) >> 8) & 0x03}, //window width high 2 bit
	{0x10, (WINDOW_WIDTH + 8) & 0xFF}, //window width low 8 bit
	{0x11, 0x2a}, //sh_delay, default: 0x2a
	{0x12, 0x06}, //Vs_st, number of Row time from frame start to first HSYNC valid, default: 0x04
	{0x13, 0x06}, //Vs_et, number of Row time from last HSYNC valid to frame end Notice the relation with VB, VB > vs_st+vs_et, default: 0x04

  {0x17, 0x80 | (COLUMN_FLIP << 1) | ROW_FLIP}, //[7] HSYNC always; [5:4] CFA sequence; [3:2] dark CFA sequence; [1] Updown (y-axis/height swap); [0] mirror (x-axis/width swap);
  {0x19, 0x05}, //Reserved
  {0x1b, 0x24},
  {0x1c, 0x04},
  {0x1e, 0x08},
  {0x1f, 0x08}, //Reserved //C8
  {0x20, 0x00}, // 0x01
  {0x21, 0x40}, //Reserved //0x48
  {0x22, 0xba},
  {0x23, 0x22},
  {0x24, 0x3f},
//////////////////blk////////////////////
  {0x26, 0xf7}, // [7] dark current mode; [6:4] BLK smooth speed; [3:2] BLK row select mode; [1] dark current en; [0] offset en;
  {0x28, 0x7f}, // BLK limit
  {0x29, 0x00}, // global offset
  {0x32, 0x04}, // 04 darkc
  {0x33, 0x18}, // blk ratio
  {0x34, 0x18},
  {0x35, 0x18},
  {0x36, 0x18},

  {0x3b, 0x00}, // manual R1 offset
  {0x3c, 0x00}, // manual G1 offset
  {0x3d, 0x00}, // manual G2 offset
  {0x3e, 0x00}, // manual B2 offset
////////////////////ISP BLOCK ENABLE////////////////////
  {0x40, 0xff},
  {0x41, 0x24},//[5]skin detection
  {0x42, 0xfa},//disable ABS
  {0x46, 0x02},//Synchronize signal output mode
  {0x4b, 0xca}, //[1] AWB_gain_mode, [0] more boundary mode
  {0x4d, 0x01},
	// crop
  {0x50, 0x01}, // [7:1] NA, [0] crop out window mode.
	{0x51, (((((WINDOW_HEIGHT + 16) / SUB_SAMPLE_HEIGHT) - IMG_HEIGHT) >> 1) >> 8) & 0x01}, // [1:0]Crop_win_y1[9:8], Bit[9]: 0 -> [8:0] is valid, forward; 1 -> [5:0] is valid, backward
	{0x52, ((((WINDOW_HEIGHT + 16) / SUB_SAMPLE_HEIGHT) - IMG_HEIGHT) >> 1) & 0xFF}, // Crop_win_y1[7:0]
	{0x53, ((((WINDOW_WIDTH / SUB_SAMPLE_WIDTH) - IMG_WIDTH) >> 1) >> 8) & 0x03}, // [2:0]Crop_win_x1[10:8], Bit[10]: 0 -> [9:0] is valid, forward; 1 -> [3:0] is valid, backward
	{0x54, (((WINDOW_WIDTH / SUB_SAMPLE_WIDTH) - IMG_WIDTH) >> 1) & 0xFF}, // Crop_win_x1[7:0]
  {0x55, (IMG_HEIGHT >> 8) & 0x01}, //out window height[8]
  {0x56, (IMG_HEIGHT & 0xFF)}, //out window height[7:0]
	{0x57, (IMG_WIDTH >> 8) & 0x03}, //out window width[9:8]
  {0x58, (IMG_WIDTH & 0xFF)}, //out window width[7:0]
	{0x59, (SUB_SAMPLE_HEIGHT << 4) | (SUB_SAMPLE_WIDTH & 0x0F)}, //[7:4] subsample row ratio, [3:0] subsample col ratio
	{0x5a, 0x0e}, //[5]use or cut row [4]use or cut col [3]vacancy zero mode [2]remove 00 mode [1]neighbor average mode [0]subsample extend pclk
	{0x5b, 0x00}, //[7:4] sub_row_num1 [3:0] sub_row_num2
	{0x5c, 0x00}, //[7:4] sub_row_num3 [3:0] sub_row_num4
	{0x5d, 0x00}, //[7:4] sub_row_num5 [3:0] sub_row_num6
	{0x5e, 0x00}, //[7:4] sub_row_num7 [3:0] sub_row_num8
	{0x5f, 0x00}, //[7:4] sub_col_num1 [3:0] sub_col_num2
	{0x60, 0x00}, //[7:4] sub_col_num3 [3:0] sub_col_num4
	{0x61, 0x00}, //[7:4] sub_col_num5 [3:0] sub_col_num6
	{0x62, 0x00}, //[7:4] sub_col_num7 [3:0] sub_col_num8

////////////////////DNDD////////////////////
  {0x80, 0x07}, // 0xe7 20140915
  {0x81, 0xc2}, // 0x22 20140915
  {0x82, 0x90}, //DN auto DNDD DEC DNDD
  {0x83, 0x05},
  {0x87, 0x40}, //0x4a  20140915
//////////////////INTPEE////////////////////
  {0x90, 0x8c}, //ac
  {0x92, 0x05},
  {0x94, 0x05},
	/* Edge effect */
  {0x95, 0x44}, // edge1/2 effect, Controlled by user or ASDE
  {0x96, 0x88}, //[7:4] positive edge ratio, 1.3bits [3:0] negative edge ratio, 1.3bits
	{0x97, 0x81}, //[7:4] edge1 max [3:0] edge1 min
	{0x98, 0x81}, //[7:4] edge2 max [3:0] edge2 min
	{0x99, 0x22}, //[7:4] edge1 threshold [3:0] edge2 threshold
	{0x9a, 0xff}, //[7:4] positive edge max [3:0] negative edge max
////////////////////ASDE////////////////////
//	/*---------- Page 1 ----------*/
  {0xfe, 0x01}, //page select - 1
  {0x18, 0x05}, // AEC_luma_div [7:4] for AWB; [3:0] for ASDE
	/*---------- Page 0 ----------*/
  {0xfe, 0x00}, //page select - 0
  {0x9c, 0x06},
	{0x9e, 0xea},
  {0xa0, 0x5f},
  {0xa2, 0x12},
  {0xa4, 0x10}, //auto saturation decrease slope
  {0xa5, 0x31}, //[7:4] auto saturation low limit, [3:0] sub saturation slope
  {0xa7, 0x60}, //ASDE low luminance value threshold
////////////////////RGB gamma////////////////////
  {0xbf, 0x10},
  {0xc0, 0x20},
  {0xc1, 0x38},
  {0xc2, 0x4e},
  {0xc3, 0x63},
  {0xc4, 0x76},
  {0xc5, 0x87},
  {0xc6, 0xa2},
  {0xc7, 0xb8},
  {0xc8, 0xca},
  {0xc9, 0xd8},
  {0xca, 0xe3},
  {0xcb, 0xe9},
  {0xcc, 0xf0},
  {0xcd, 0xf8},
  {0xce, 0xfd},
  {0xcf, 0xff},
////////////////////Y gamma//////////////////
  {0x63, 0x00},
  {0x64, 0x10},
  {0x65, 0x1c},
  {0x66, 0x30},
  {0x67, 0x43},
  {0x68, 0x54},
  {0x69, 0x65},
  {0x6a, 0x75},
  {0x6b, 0x93},
  {0x6c, 0xb0},
  {0x6d, 0xcb},
  {0x6e, 0xe6},
  {0x6f, 0xff},
//////////////////CC///////////////////
  {0xb3, 0x44},
  {0xb4, 0xfd},
  {0xb5, 0x02},
  {0xb6, 0xfa},
  {0xb7, 0x48},
  {0xb8, 0xf0},
////////////////////YCP////////////////////
  {0xd0, 0x40},
  {0xd1, 0x40},
  {0xd2, 0x40},
  /* luma contrast control */
  {0xd3, 0x80}, // luma_contrast, can be adjusted via contrast center
	{0xd4, 0x80}, // Contrast center value
  {0xd5, 0x00}, // Add offset on luma value

	{0xd8, 0x28}, // Defines skin range
	{0xd9, 0xe3}, // [7:4]skin grightness high threshold, [3:0]skin brightness low threshold
	
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
////////////////////AWB//////////////////////
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
////////////////////CC-AWB///////////////////
  {0xd0, 0x00},
  {0xd2, 0x2c},
  {0xd3, 0x80},
///////////////////LSC //////////////////////
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
  {0x29, 0x00}, //anti-flicker step [11:8]
  {0x2a, 0xa0}, //anti-flicker step [7:0]
/*
	// test on windows(240 * 240), image(120 * 120)
	0x000 -> 59.5243Hz
	0x100 -> 59.5243Hz
	0x1ac -> 50.0672Hz
	0x1ca -> 46.7876Hz
	0x200 -> 41.8530Hz
	0x218 -> 39.9790Hz
	0x2ca -> 30.0122Hz
	0x300 -> 27.9020Hz
	0x3c0 -> 22.3216Hz
	0x400 -> 20.9265Hz
	0x42f -> 20.0082Hz
	*/
  {0x2b, 0x02}, //exp level 0  50fps
  {0x2c, 0x18},
  {0x2d, 0x02}, //exp level 1  40fps
  {0x2e, 0xca},
  {0x2f, 0x04}, //exp level 2  30fps
  {0x30, 0x2f},
  {0x31, 0x05}, //exp level 3  20fps
  {0x32, 0x20},
	{0x33, 0x21}, //[5:4] Max level setting, [3:0] exp_min[11:8]
	{0x34, 0x00}, //exp_min[7:0]
	/*---------- Page 0 ----------*/
  {0xfe, 0x00}, //page select - 0
////////////////////out ///////////////////
	{0x43, 0x00}, // [1] CbCr fixed enable; [0] Inverse color
	{0x44, 0x00 | IMG_FORMAT}, //output format: only Y
//	{0x4e, 0x09},
  {0xf0, 0x07}, //pclk_en && hsync_en && vsync_en
  {0xf1, 0x01}, //normal data output enable
//	{0xf3, 0xa8}, //sync & pclk & data -> pull up, PWDN_DN -> pull down.
//	{0xf5, 0x00}, //rec_bandwidth
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
	CAMERA_IO_Write(DeviceAddr, 0x51, (value >> 8) & 0x01);
	CAMERA_IO_Write(DeviceAddr, 0x52, value & 0xFF);
	CAMERA_IO_Write(DeviceAddr, 0x53, (value >> 24) & 0x03);
	CAMERA_IO_Write(DeviceAddr, 0x54, (value >> 16) & 0xFF);
	CAMERA_IO_Write(DeviceAddr, 0x05, (brightness_value >> 16) & 0x03);
	CAMERA_IO_Write(DeviceAddr, 0x06, (brightness_value >> 8) & 0xFF);
	CAMERA_IO_Write(DeviceAddr, 0x11, brightness_value & 0xFF);
	CAMERA_IO_Write(DeviceAddr, 0x07, (feature >> 24) & 0xFF);
	CAMERA_IO_Write(DeviceAddr, 0x08, (feature >> 16) & 0xFF);
	CAMERA_IO_Write(DeviceAddr, 0x12, (feature >> 8) & 0xFF);
	CAMERA_IO_Write(DeviceAddr, 0x13, feature & 0xFF);
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
