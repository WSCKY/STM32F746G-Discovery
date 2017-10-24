/**
  *******************************************************************************
  * @file    BSPTest/LED/main/main.c 
  * @author  '^_^'
  * @version V0.0.1
  * @date    15-October-2015
  * @brief   This file provides main program functions
  *******************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "main.h"
#include <math.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/**
  * @brief  LCD FB_StartAddress
  * LCD Frame buffer start address : starts at beginning of SDRAM
  */
#define LCD_FRAME_BUFFER          SDRAM_DEVICE_ADDR

typedef struct
{
	float Acc, Vel, velMax, Pos, LastPos, expPos, dt;
}QuadCurve;//element Acc,velMax should be > 0, all element has the same unit.

typedef struct
{
	uint16_t X, Y;
}PixelPos;
/* Private macro -------------------------------------------------------------*/
#define GETDIR(x, y) (x > y ? (1) : (x < y ? (-1) : (0)))
#define SGN(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))

/* Private variables ---------------------------------------------------------*/
uint32_t TimeIndex = 0;
QuadCurve TestCurve = {0};
PixelPos CurPos, LastPos;

uint8_t BreakPoint = 0;
uint8_t ret = 0, Lastret = 0;
/* Global extern variables ---------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void CPU_CACHE_ENABLE(void);
/* Private functions ---------------------------------------------------------*/
static uint8_t QuadCurveCtrl(QuadCurve *pCurve, uint8_t Condition);
float constrain(float fData, float Min, float Max);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	CPU_CACHE_ENABLE();
  /* STM32F7xx HAL library initialization:
	 - Configure the Flash prefetch, instruction and Data caches
     - Configure the Systick to generate an interrupt each 1 msec
     - Set NVIC Group Priority to 4
	 - Global MSP (MCU Support Package) initialization
  */
	HAL_Init();
	/* Configure the system clock to 200 Mhz */
	SystemClock_Config();

	BSP_LED_Init(LED1);
	/* Initialize the LCD */
	BSP_LCD_Init();
	/* Initialize the LCD Layers */
	BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FRAME_BUFFER);

	/* Set LCD Foreground Layer  */
	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);

	BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

	/* Clear the LCD */
	BSP_LCD_SetBackColor(0xFF111111);
	BSP_LCD_Clear(0xFF111111);
	/* Set the LCD Text Color */
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);

	//Curve Init.
	TestCurve.Acc = 0.05f;
	TestCurve.dt = 1.0f;
	TestCurve.velMax = 0.0f;
	TestCurve.Pos = 0;//130;
	TestCurve.expPos = 250;

	CurPos.X = LastPos.X = TimeIndex;
	CurPos.Y = LastPos.Y = TestCurve.Pos;

	while(TimeIndex < 480)
	{
		if(TimeIndex % 20 == 0)
			BreakPoint ++;
//		if((TimeIndex / 80) % 2 == 0)
//		{
//			TestCurve.expPos = 250;// - (TimeIndex / 80) * 7;
//			BSP_LCD_DrawPixel(TimeIndex, (uint16_t)TestCurve.expPos, LCD_COLOR_LIGHTBLUE);
//		}
//		else
//		{
//			TestCurve.expPos = 10;// + (TimeIndex / 80) * 7;
//			BSP_LCD_DrawPixel(TimeIndex, (uint16_t)TestCurve.expPos, LCD_COLOR_LIGHTBLUE);
//		}
////		if(TimeIndex % 40 == 0)
////			BSP_LCD_DrawVLine(TimeIndex, 10, 240);

		ret = QuadCurveCtrl(&TestCurve, ((TimeIndex < 80) || (TimeIndex > 360)));
		if((ret == 0 && Lastret == 1) || (TimeIndex == 80) || (TimeIndex == 360))
		{
			BSP_LCD_SetTextColor(LCD_COLOR_RED);
			BSP_LCD_DrawVLine(TimeIndex, 10, 240);
		}
		Lastret = ret;
		CurPos.X = TimeIndex;
		CurPos.Y = TestCurve.Pos;//(uint16_t)constrain(TestCurve.Pos, 0, 271);
		BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGREEN);
		BSP_LCD_DrawLine(LastPos.X, LastPos.Y, CurPos.X, CurPos.Y);
		BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
		LastPos = CurPos;
//		BSP_LCD_DrawPixel(TimeIndex, (uint16_t)constrain(TestCurve.Pos, 0, 271), LCD_COLOR_LIGHTGREEN);
		TimeIndex ++;
	}

	while(1)
	{
		BSP_LED_Toggle(LED1);
		HAL_Delay(200);
	}
}

float constrain(float fData, float Min, float Max)
{
	return ((fData > Max) ? Max : ((fData < Min) ? Min : fData));
}

/**
  * @brief  quadric curve control function.
  * @param  *pCurve: pointer to a QuadCurve structure.
  * @retval Running status.
  */
uint8_t QuadCurveCtrl(QuadCurve *pCurve, uint8_t Condition)
{
	float DisToExp = fabsf(pCurve->expPos - pCurve->Pos);//distance to expectation.
	float DecDist = (pCurve->Vel * pCurve->Vel) / (2 * pCurve->Acc);//deceleration distance.
	float dVel = pCurve->Acc * pCurve->dt * GETDIR(pCurve->expPos, pCurve->Pos);
	if(pCurve->expPos == pCurve->Pos || ((pCurve->expPos - pCurve->Pos) * (pCurve->expPos - pCurve->LastPos) < 0))//stop point.
	{
		pCurve->Vel = 0;
		pCurve->LastPos = pCurve->Pos;
		pCurve->Pos = pCurve->expPos;
		return 0;//got expPos.
	}
	if(DisToExp <= DecDist || (Condition == 0))//close to expectation
	{
		if(Condition == 1)
		{
			float A = (pCurve->Vel * pCurve->Vel) / (2 * DisToExp);
			dVel = A * pCurve->dt * GETDIR(pCurve->expPos, pCurve->Pos);
		}
		if(fabsf(pCurve->Vel) > 2 * fabsf(dVel))
			pCurve->Vel -= 2 * fabsf(dVel) * SGN(pCurve->Vel);
		else
		{
			pCurve->Vel = 0;
			return 0;
		}
	}
	else
	{
		if(pCurve->velMax != 0)//has velmax limit.
		{
			if(fabsf(pCurve->Vel + dVel) < pCurve->velMax)
				pCurve->Vel += dVel;
			else
				pCurve->Vel = SGN(pCurve->Vel) * pCurve->velMax;
		}
		else
			pCurve->Vel += dVel;
	}

	pCurve->LastPos = pCurve->Pos;
	pCurve->Pos += pCurve->Vel * pCurve->dt;

	return 1;
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 200000000
  *            HCLK(Hz)                       = 200000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 400
  *            PLL_P                          = 2
  *            PLLSAI_N                       = 384
  *            PLLSAI_P                       = 8
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
          
  ret = HAL_PWREx_EnableOverDrive();

  if(ret != HAL_OK)
  {
    while(1) { ; }
  }  
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

/**
  * @brief  CPU L1-Cache enable.
  *         Invalidate Data cache before enabling
  *         Enable Data & Instruction Cache
  * @param  None
  * @retval None
  */
static void CPU_CACHE_ENABLE(void)
{
  (*(uint32_t *) 0xE000ED94) &= ~0x5;
  (*(uint32_t *) 0xE000ED98) = 0x0; //MPU->RNR
  (*(uint32_t *) 0xE000ED9C) = 0x20010000 |1<<4; //MPU->RBAR
  (*(uint32_t *) 0xE000EDA0) = 0<<28 | 3 <<24 | 0<<19 | 0<<18 | 1<<17 | 0<<16 | 0<<8 | 30<<1 | 1<<0 ; //MPU->RASE  WT
  (*(uint32_t *) 0xE000ED94) = 0x5;

  /* Invalidate I-Cache : ICIALLU register*/
  SCB_InvalidateICache();	
	
  /* Enable branch prediction */
  SCB->CCR |= (1 <<18); 
  __DSB();

  /* Enable I-Cache */
  SCB_EnableICache();	
	
  /* Enable D-Cache */
  SCB_InvalidateDCache();
  SCB_EnableDCache();
}

#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File: pointer to the source file name
* @param  Line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line
  number,ex: printf("Wrong parameters value: file %s on line %d\r\n", 
  file, line) */
  
  /* Infinite loop */
  while (1);
}

#endif

/********************************* END OF FILE *********************************/
