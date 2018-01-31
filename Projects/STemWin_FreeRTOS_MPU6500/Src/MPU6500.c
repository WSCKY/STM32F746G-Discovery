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
#include "MPU6500.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;

uint8_t mpu_id = 0xFF; /* mpu_id = 0x70 */
/* Private function prototypes -----------------------------------------------*/
static void MPU6500_GPIO_Init(void);
static int8_t MPU6500_SPI_Init(void);
static int8_t MPU6500_ReadReg(uint8_t RegAddr, uint8_t len, uint8_t *p);
static int8_t MPU6500_WriteReg(uint8_t RegAddr, uint8_t RegData);
/* Private functions ---------------------------------------------------------*/

int8_t MPU6500_Init(void)
{
	HAL_Delay(50);
	MPU6500_GPIO_Init();
	if(MPU6500_SPI_Init() != 0) return -1;

	MPU6500_WriteReg(0x6B, 0x80);
	HAL_Delay(10);
	MPU6500_WriteReg(0x68, 0x07);
	MPU6500_WriteReg(0x6B, 0x00);
	HAL_Delay(10);
	MPU6500_WriteReg(0x19, 0x00);
	MPU6500_WriteReg(0x1A, 0x03);
	MPU6500_WriteReg(0x1D, 0x06);
	MPU6500_WriteReg(0x1C, 0x10);
	MPU6500_WriteReg(0x1B, 0x18);
	HAL_Delay(1);
	MPU6500_ReadReg(0x75, 1, (uint8_t *)&mpu_id);
	if(mpu_id != 0x70) return -1;
	return 0;
}

int8_t MPU6500_VerifyID(void)
{
	uint8_t _rx_data = 0;
	if(MPU6500_ReadReg(0x75, 1, &_rx_data) != 0) return -1;
	if(_rx_data != 0x70) return -1;
	return 0;
}

static void MPU6500_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	MPU6500_SPI_SCK_GPIO_CLK_ENABLE();
	MPU6500_SPI_MISO_GPIO_CLK_ENABLE();
	MPU6500_SPI_MOSI_GPIO_CLK_ENABLE();
	/* Enable SPI clock */
	MPU6500_SPI_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* SPI SCK GPIO pin configuration  */
	GPIO_InitStruct.Pin       = MPU6500_SPI_SCK_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = MPU6500_SPI_SCK_AF;
	HAL_GPIO_Init(MPU6500_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);

	/* SPI MISO GPIO pin configuration  */
	GPIO_InitStruct.Pin = MPU6500_SPI_MISO_PIN;
	GPIO_InitStruct.Alternate = MPU6500_SPI_MISO_AF;
	HAL_GPIO_Init(MPU6500_SPI_MISO_GPIO_PORT, &GPIO_InitStruct);

	/* SPI MOSI GPIO pin configuration  */
	GPIO_InitStruct.Pin = MPU6500_SPI_MOSI_PIN;
	GPIO_InitStruct.Alternate = MPU6500_SPI_MOSI_AF;
	HAL_GPIO_Init(MPU6500_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);

	/* Configure Control Pins */
	MPU6500_SPI_CS_GPIO_CLK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Alternate = 0;
	GPIO_InitStruct.Pin = MPU6500_SPI_CS_PIN;
	HAL_GPIO_Init(MPU6500_SPI_CS_GPIO_PORT, &GPIO_InitStruct);

	/* -3- Drive NCS high, and then low to reset the SPI port. */
	HAL_GPIO_WritePin(MPU6500_SPI_CS_GPIO_PORT, MPU6500_SPI_CS_PIN, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(MPU6500_SPI_CS_GPIO_PORT, MPU6500_SPI_CS_PIN, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(MPU6500_SPI_CS_GPIO_PORT, MPU6500_SPI_CS_PIN, GPIO_PIN_SET);
}

static int8_t MPU6500_SPI_Init(void)
{
	/*##-1- Configure the SPI peripheral #######################################*/
  /* Set the SPI parameters */
  SpiHandle.Instance               = MPU6500_SPI;
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

static int8_t MPU6500_ReadReg(uint8_t RegAddr, uint8_t len, uint8_t *p)
{
	int8_t ret = 0;
	uint8_t _tx_data[20] = {0};
	_tx_data[0] = RegAddr | 0x80;
	HAL_GPIO_WritePin(MPU6500_SPI_CS_GPIO_PORT, MPU6500_SPI_CS_PIN, GPIO_PIN_RESET);

	if(HAL_SPI_TransmitReceive(&SpiHandle, _tx_data, p, 1, 50) != HAL_OK) ret = -1;
	_tx_data[0] = 0;
	if(HAL_SPI_TransmitReceive(&SpiHandle, _tx_data, p, len, 50 * len) != HAL_OK) ret = -1;

	HAL_GPIO_WritePin(MPU6500_SPI_CS_GPIO_PORT, MPU6500_SPI_CS_PIN, GPIO_PIN_SET);
	return ret;
}

static int8_t MPU6500_WriteReg(uint8_t RegAddr, uint8_t RegData)
{
	int8_t ret = 0;
	uint8_t _rx_data = 0;
	uint8_t _tx_data = RegAddr;
	HAL_GPIO_WritePin(MPU6500_SPI_CS_GPIO_PORT, MPU6500_SPI_CS_PIN, GPIO_PIN_RESET);

	if(HAL_SPI_TransmitReceive(&SpiHandle, &_tx_data, &_rx_data, 1, 50) != HAL_OK) ret = -1;
	_tx_data = RegData;
	if(HAL_SPI_TransmitReceive(&SpiHandle, &_tx_data, &_rx_data, 1, 50) != HAL_OK) ret = -1;

	HAL_GPIO_WritePin(MPU6500_SPI_CS_GPIO_PORT, MPU6500_SPI_CS_PIN, GPIO_PIN_SET);
	return ret;
}
