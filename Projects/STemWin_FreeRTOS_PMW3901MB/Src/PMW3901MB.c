/**
  ******************************************************************************
  * @file    Project/Src/main.c
  * @author  kyChu
  * @version V1.0.0
  * @date    15-December-2017
  * @brief   This file provides Driver for PMW3901MB Optical Motion Tracking Sensor.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "PMW3901MB.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define REG_PRODUCT_ID                 0x00 /* RO */
#define REG_REVISION_ID                0x01 /* RO */
#define REG_MOTION                     0x02 /* RW */
#define REG_DELTA_X_L                  0x03 /* RO */
#define REG_DELTA_X_H                  0x04 /* RO */
#define REG_DELTA_Y_L                  0x05 /* RO */
#define REG_DELTA_Y_H                  0x06 /* RO */
#define REG_SQUAL                      0x07 /* RO */
#define REG_RAW_DATA_SUM               0x08 /* RO */
#define REG_MAX_RAW_DATA               0x09 /* RO */
#define REG_MIN_RAW_DATA               0x0A /* RO */
#define REG_SHUTTER_LOWER              0x0B /* RO */
#define REG_SHUTTER_UPPER              0x0C /* RO */
#define REG_OBSERVATION                0x15 /* RW */
#define REG_MOTION_BURST               0x16 /* RO */
#define REG_POWER_UP_RESET             0x3A /* WO */
#define REG_SHUTDOWN                   0x3B /* WO */
#define REG_RAW_DATA_GRAB              0x58 /* RW */
#define REG_RAW_DATA_GRAB_STATUS       0x59 /* RO */
#define REG_INVERSE_PRODUCT_ID         0x5F /* RO */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;

uint8_t PerfOptReg_1[59][2] = {
{0x7F, 0x00},
{0x61, 0xAD},
{0x7F, 0x03},
{0x40, 0x00},
{0x7F, 0x05},
{0x41, 0xB3},
{0x43, 0xF1},
{0x45, 0x14},
{0x5B, 0x32},
{0x5F, 0x34},
{0x7B, 0x08},
{0x7F, 0x06},
{0x44, 0x1B},
{0x40, 0xBF},
{0x4E, 0x3F},
{0x7F, 0x08},
{0x65, 0x20},
{0x6A, 0x18},
{0x7F, 0x09},
{0x4F, 0xAF},
{0x5F, 0x40},
{0x48, 0x80},
{0x49, 0x80},
{0x57, 0x77},
{0x60, 0x78},
{0x61, 0x78},
{0x62, 0x08},
{0x63, 0x50},
{0x7F, 0x0A},
{0x45, 0x60},
{0x7F, 0x00},
{0x4D, 0x11},
{0x55, 0x80},
{0x74, 0x1F},
{0x75, 0x1F},
{0x4A, 0x78},
{0x4B, 0x78},
{0x44, 0x08},
{0x45, 0x50},
{0x64, 0xFF},
{0x65, 0x1F},
{0x7F, 0x14},
{0x65, 0x60},
{0x66, 0x08},
{0x63, 0x78},
{0x7F, 0x15},
{0x48, 0x58},
{0x7F, 0x07},
{0x41, 0x0D},
{0x43, 0x14},
{0x4B, 0x0E},
{0x45, 0x0F},
{0x44, 0x42},
{0x4C, 0x80},
{0x7F, 0x10},
{0x5B, 0x02},
{0x7F, 0x07},
{0x40, 0x41},
{0x70, 0x00}
};
/* Delay 10ms */
uint8_t PerfOptReg_2[14][2] = {
{0x32, 0x44},
{0x7F, 0x07},
{0x40, 0x40},
{0x7F, 0x06},
{0x62, 0xF0},
{0x63, 0x00},
{0x7F, 0x0D},
{0x48, 0xC0},
{0x6F, 0xD5},
{0x7F, 0x00},
{0x5B, 0xA0},
{0x4E, 0xA8},
{0x5A, 0x50},
{0x40, 0x80}
};

static uint8_t _init_done = 0;
static uint8_t _data_update = 0;
static PMW3901MB_BurstReportDef _raw_data = {0};
/* Private function prototypes -----------------------------------------------*/
static void PMW3901_GPIO_Init(void);
static int8_t PMW3901_SPI_Init(void);
static int8_t PMW3901_ReadReg(uint8_t RegAddr, uint8_t len, uint8_t *p);
static int8_t PMW3901_WriteReg(uint8_t RegAddr, uint8_t RegData);
/* Private functions ---------------------------------------------------------*/

int8_t PMW3901_Init(void)
{
	uint8_t i = 0, _rx_data = 0;
	/* -2- Wait for at least 40ms. */
	HAL_Delay(50);
	PMW3901_GPIO_Init();
	if(PMW3901_SPI_Init() != 0) return -1;

	/* -4- Write 0x5A to Power_Up_Reset register (or alternatively, toggle the NRESET pin). */
	if(PMW3901_WriteReg(REG_POWER_UP_RESET, 0x5A) != 0) return -1;
	/* -5- Wait for at least 1ms. */
	HAL_Delay(1);
	/* -6- Read from registers 0x02, 0x03, 0x04, 0x05 and 0x06 one time regardless of the motion pin state. */
	ReadDeltaDataRaw();
	/* -7- Refer Section '8.2 Performance Optimization Registers' to configure the needed registers in order to achieve optimum performance of the chip. */
	for(i = 0; i < 59; i ++) {
		if(PMW3901_WriteReg(PerfOptReg_1[i][0], PerfOptReg_1[i][1]) != 0) return -1;
		HAL_Delay(1);
	}
	HAL_Delay(10);
	for(i = 0; i < 14; i ++) {
		if(PMW3901_WriteReg(PerfOptReg_2[i][0], PerfOptReg_2[i][1]) != 0) return -1;
		HAL_Delay(1);
	}
	HAL_Delay(5);
	if(PMW3901_ReadReg(REG_PRODUCT_ID, 1, &_rx_data) != 0) return -1;
	if(_rx_data != 0x49) return -1;
	HAL_Delay(1);
	if(PMW3901_ReadReg(REG_INVERSE_PRODUCT_ID, 1, &_rx_data) != 0) return -1;
	if(_rx_data != 0xB6) return -1;
	_init_done = 1;
	return 0;
}

int8_t PMW3901_VerifyID(void)
{
	uint8_t _rx_data = 0;
	if(PMW3901_ReadReg(REG_PRODUCT_ID, 1, &_rx_data) != 0) return -1;
	if(_rx_data != 0x49) return -1;
	HAL_Delay(1);
	if(PMW3901_ReadReg(REG_INVERSE_PRODUCT_ID, 1, &_rx_data) != 0) return -1;
	if(_rx_data != 0xB6) return -1;
	return 0;
}

PMW3901MB_BurstReportDef *ReadDeltaDataRaw(void)
{
	return &_raw_data;
}

uint8_t PMW3901_DataUpdated(void)
{
	if(_data_update) {
		_data_update = 0;
		return 1;
	}
	return 0;
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == PMW3901_MOTION_PIN) {
		if(_init_done) {
			/*
			 * Reading the Motion_Burst register activates Burst Mode. PMW3901MB will respond with the motion burst report in order.
			 */
			PMW3901_ReadReg(REG_MOTION_BURST, 12, (uint8_t *)&_raw_data);
			_data_update = 1;
		}
	}
}

static void PMW3901_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	PMW3901_SPI_SCK_GPIO_CLK_ENABLE();
	PMW3901_SPI_MISO_GPIO_CLK_ENABLE();
	PMW3901_SPI_MOSI_GPIO_CLK_ENABLE();
	/* Enable SPI clock */
	PMW3901_SPI_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* SPI SCK GPIO pin configuration  */
	GPIO_InitStruct.Pin       = PMW3901_SPI_SCK_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = PMW3901_SPI_SCK_AF;
	HAL_GPIO_Init(PMW3901_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);

	/* SPI MISO GPIO pin configuration  */
	GPIO_InitStruct.Pin = PMW3901_SPI_MISO_PIN;
	GPIO_InitStruct.Alternate = PMW3901_SPI_MISO_AF;
	HAL_GPIO_Init(PMW3901_SPI_MISO_GPIO_PORT, &GPIO_InitStruct);

	/* SPI MOSI GPIO pin configuration  */
	GPIO_InitStruct.Pin = PMW3901_SPI_MOSI_PIN;
	GPIO_InitStruct.Alternate = PMW3901_SPI_MOSI_AF;
	HAL_GPIO_Init(PMW3901_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);

	/* Configure Control Pins */
	PMW3901_SPI_CS_GPIO_CLK_ENABLE();
	PMW3901_NRST_GPIO_CLK_ENABLE();
	PMW3901_MOTION_GPIO_CLK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Alternate = 0;
	GPIO_InitStruct.Pin = PMW3901_SPI_CS_PIN;
	HAL_GPIO_Init(PMW3901_SPI_CS_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PMW3901_NRST_PIN;
	HAL_GPIO_Init(PMW3901_NRST_GPIO_PORT, &GPIO_InitStruct);

	/* Configure Motion pin as input with External interrupt */
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Pin = PMW3901_MOTION_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	HAL_GPIO_Init(PMW3901_MOTION_GPIO_PORT, &GPIO_InitStruct);
	/* Enable and set MOTION EXTI Interrupt to the lowest priority */
	HAL_NVIC_SetPriority(PMW3901_MOTION_EXTI_IRQn, 0x0F, 0x00);
	HAL_NVIC_EnableIRQ(PMW3901_MOTION_EXTI_IRQn);

	/* -3- Drive NCS high, and then low to reset the SPI port. */
	HAL_GPIO_WritePin(PMW3901_SPI_CS_GPIO_PORT, PMW3901_SPI_CS_PIN, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(PMW3901_SPI_CS_GPIO_PORT, PMW3901_SPI_CS_PIN, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(PMW3901_SPI_CS_GPIO_PORT, PMW3901_SPI_CS_PIN, GPIO_PIN_SET);

	HAL_GPIO_WritePin(PMW3901_NRST_GPIO_PORT, PMW3901_NRST_PIN, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(PMW3901_NRST_GPIO_PORT, PMW3901_NRST_PIN, GPIO_PIN_SET);
	HAL_Delay(10);
}

static int8_t PMW3901_SPI_Init(void)
{
	/*##-1- Configure the SPI peripheral #######################################*/
  /* Set the SPI parameters */
  SpiHandle.Instance               = PMW3901_SPI;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
  SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  SpiHandle.Init.CRCPolynomial     = 7;
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;
	SpiHandle.Init.Mode              = SPI_MODE_MASTER;

	if(HAL_SPI_Init(&SpiHandle) != HAL_OK) {
		return -1;
	}
	return 0;
}

static int8_t PMW3901_ReadReg(uint8_t RegAddr, uint8_t len, uint8_t *p)
{
	int8_t ret = 0;
	uint8_t _tx_data[20] = {0};
	_tx_data[0] = RegAddr;
	HAL_GPIO_WritePin(PMW3901_SPI_CS_GPIO_PORT, PMW3901_SPI_CS_PIN, GPIO_PIN_RESET);

	if(HAL_SPI_TransmitReceive(&SpiHandle, _tx_data, p, 1, 50) != HAL_OK) ret = -1;
	_tx_data[0] = 0;
	if(HAL_SPI_TransmitReceive(&SpiHandle, _tx_data, p, len, 50 * len) != HAL_OK) ret = -1;

	HAL_GPIO_WritePin(PMW3901_SPI_CS_GPIO_PORT, PMW3901_SPI_CS_PIN, GPIO_PIN_SET);
	return ret;
}

static int8_t PMW3901_WriteReg(uint8_t RegAddr, uint8_t RegData)
{
	int8_t ret = 0;
	uint8_t _rx_data = 0;
	uint8_t _tx_data = RegAddr | 0x80;
	HAL_GPIO_WritePin(PMW3901_SPI_CS_GPIO_PORT, PMW3901_SPI_CS_PIN, GPIO_PIN_RESET);

	if(HAL_SPI_TransmitReceive(&SpiHandle, &_tx_data, &_rx_data, 1, 50) != HAL_OK) ret = -1;
	_tx_data = RegData;
	if(HAL_SPI_TransmitReceive(&SpiHandle, &_tx_data, &_rx_data, 1, 50) != HAL_OK) ret = -1;

	HAL_GPIO_WritePin(PMW3901_SPI_CS_GPIO_PORT, PMW3901_SPI_CS_PIN, GPIO_PIN_SET);
	return ret;
}
