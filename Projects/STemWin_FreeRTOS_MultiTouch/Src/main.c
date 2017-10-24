/**
  ******************************************************************************
  * @file    STemWin/STemWin_HelloWorld/Src/main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    25-June-2015
  * @brief   This file provides main program functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "WM.h"
#include "cmsis_os.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t GUI_Initialized = 0;
osThreadId TaskThreadId, STemWinId, TSC_TimerId;
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Start_Thread(void const *argument);
static void STemWinThread(void const *argument);
static void TimerCallback(void const *n);
extern void MainTask(void);
static void CPU_CACHE_Enable(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */ 
int main(void)
{
	/* Enable the CPU Cache */
	CPU_CACHE_Enable();

	/* STM32F7xx HAL library initialization:
       - Configure the Flash ART accelerator on ITCM interface
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
	HAL_Init();  

	/* Configure the system clock to 200 MHz */
	SystemClock_Config();

	/* Start Thread definition */
	osThreadDef(Start, Start_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);

	/* Start thread */
	TaskThreadId = osThreadCreate(osThread(Start), NULL);

	/* Start scheduler */
	osKernelStart();

	/***********************************************************/
	/* Infinite loop */
	for(;;);
}

/**
  * @brief  Task Start thread
  * @param  Thread not used
  * @retval None
  */
static void Start_Thread(void const *argument)
{
	uint32_t PreviousWakeTime;

	(void) argument;

	/* Configure LED1 */
	BSP_LED_Init(LED1);
	/* Initialize the Touch screen */
	BSP_TS_Init(420, 272);
	/* Init the STemWin GUI Library */
	BSP_SDRAM_Init(); /* Initializes the SDRAM device */
	__HAL_RCC_CRC_CLK_ENABLE(); /* Enable the CRC Module */
	GUI_Init();

	GUI_MTOUCH_Enable(1);
	GUI_DispStringAt("Starting...", 0, 0);

	GUI_Initialized = 1;

	/* Activate the use of memory device feature */
	WM_SetCreateFlags(WM_CF_MEMDEV);

//	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
//	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
//	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
//	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
//	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
//	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
//	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
//	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
//	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
//	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
//	GUI_CURSOR_Show();

	/* STemWin Thread definition */
	osThreadDef(STemWin, STemWinThread, osPriorityHigh, 0, configMINIMAL_STACK_SIZE * 8);
	/* Start thread */
	STemWinId = osThreadCreate(osThread(STemWin), NULL);

	/* Create Touch screen Timer */
	osTimerDef(TS_Timer, TimerCallback);
	TSC_TimerId = osTimerCreate(osTimer(TS_Timer), osTimerPeriodic, (void *)0);

	/* Start the TS Timer */
	osTimerStart(TSC_TimerId, 20);

#if USE_SPRITE
	osThreadDef(GUI_Sprite, Sprite_Run, osPriorityNormal, 0, 3 * configMINIMAL_STACK_SIZE);
	osThreadCreate(osThread(GUI_Sprite), NULL);
#endif

	PreviousWakeTime = osKernelSysTick();

	for(;;)
	{
		osDelayUntil (&PreviousWakeTime, 200);
		BSP_LED_Toggle(LED1);
	}
}

static void STemWinThread(void const *argument)
{
	(void) argument;

	for(;;)
	{
		MainTask();
	}
}

/**
  * @brief  Provide the GUI with current state of the touch screen
  * @param  None
  * @retval None
  */
static void TimerCallback(void const *n)
{
	__IO TS_StateTypeDef ts;
	GUI_MTOUCH_EVENT MTouchEvent = {0};
	GUI_MTOUCH_INPUT MTouchInput[5] = {0};
	int TimeStamp = GUI_X_GetTime();
	BSP_TS_GetState((TS_StateTypeDef *)&ts);

	MTouchEvent.TimeStamp = TimeStamp;
	MTouchEvent.NumPoints = ts.touchDetected;

	MTouchInput[0].x = ts.touchX[0];
	MTouchInput[0].y = ts.touchY[0];
	MTouchInput[0].Flags = 0;
	switch(ts.touchEventId[0])
	{
		case TOUCH_EVENT_NO_EVT:
			MTouchInput[0].Id = GUI_MTOUCH_FLAG_UP;
		break;
		case TOUCH_EVENT_PRESS_DOWN:
			MTouchInput[0].Id = GUI_MTOUCH_FLAG_DOWN;
		break;
		case TOUCH_EVENT_CONTACT:
			MTouchInput[0].Id = GUI_MTOUCH_FLAG_MOVE;
		break;
		case TOUCH_EVENT_LIFT_UP:
			MTouchInput[0].Id = GUI_MTOUCH_FLAG_UP;
		break;
		default:
			MTouchInput[0].Id = GUI_MTOUCH_FLAG_UP;
		break;
	}

	MTouchInput[1].x = ts.touchX[1];
	MTouchInput[1].y = ts.touchY[1];
	MTouchInput[1].Flags = 1;
	switch(ts.touchEventId[1])
	{
		case TOUCH_EVENT_NO_EVT:
			MTouchInput[1].Id = GUI_MTOUCH_FLAG_UP;
		break;
		case TOUCH_EVENT_PRESS_DOWN:
			MTouchInput[1].Id = GUI_MTOUCH_FLAG_DOWN;
		break;
		case TOUCH_EVENT_CONTACT:
			MTouchInput[1].Id = GUI_MTOUCH_FLAG_MOVE;
		break;
		case TOUCH_EVENT_LIFT_UP:
			MTouchInput[1].Id = GUI_MTOUCH_FLAG_UP;
		break;
		default:
			MTouchInput[1].Id = GUI_MTOUCH_FLAG_UP;
		break;
	}

	MTouchInput[2].x = ts.touchX[2];
	MTouchInput[2].y = ts.touchY[2];
	MTouchInput[2].Flags = 2;
	switch(ts.touchEventId[2])
	{
		case TOUCH_EVENT_NO_EVT:
			MTouchInput[2].Id = GUI_MTOUCH_FLAG_UP;
		break;
		case TOUCH_EVENT_PRESS_DOWN:
			MTouchInput[2].Id = GUI_MTOUCH_FLAG_DOWN;
		break;
		case TOUCH_EVENT_CONTACT:
			MTouchInput[2].Id = GUI_MTOUCH_FLAG_MOVE;
		break;
		case TOUCH_EVENT_LIFT_UP:
			MTouchInput[2].Id = GUI_MTOUCH_FLAG_UP;
		break;
		default:
			MTouchInput[2].Id = GUI_MTOUCH_FLAG_UP;
		break;
	}

	MTouchInput[3].x = ts.touchX[3];
	MTouchInput[3].y = ts.touchY[3];
	MTouchInput[3].Flags = 3;
	switch(ts.touchEventId[3])
	{
		case TOUCH_EVENT_NO_EVT:
			MTouchInput[3].Id = GUI_MTOUCH_FLAG_UP;
		break;
		case TOUCH_EVENT_PRESS_DOWN:
			MTouchInput[3].Id = GUI_MTOUCH_FLAG_DOWN;
		break;
		case TOUCH_EVENT_CONTACT:
			MTouchInput[3].Id = GUI_MTOUCH_FLAG_MOVE;
		break;
		case TOUCH_EVENT_LIFT_UP:
			MTouchInput[3].Id = GUI_MTOUCH_FLAG_UP;
		break;
		default:
			MTouchInput[3].Id = GUI_MTOUCH_FLAG_UP;
		break;
	}

	MTouchInput[4].x = ts.touchX[4];
	MTouchInput[4].y = ts.touchY[4];
	MTouchInput[4].Flags = 4;
	switch(ts.touchEventId[4])
	{
		case TOUCH_EVENT_NO_EVT:
			MTouchInput[4].Id = GUI_MTOUCH_FLAG_UP;
		break;
		case TOUCH_EVENT_PRESS_DOWN:
			MTouchInput[4].Id = GUI_MTOUCH_FLAG_DOWN;
		break;
		case TOUCH_EVENT_CONTACT:
			MTouchInput[4].Id = GUI_MTOUCH_FLAG_MOVE;
		break;
		case TOUCH_EVENT_LIFT_UP:
			MTouchInput[4].Id = GUI_MTOUCH_FLAG_UP;
		break;
		default:
			MTouchInput[4].Id = GUI_MTOUCH_FLAG_UP;
		break;
	}

	GUI_MTOUCH_StoreEvent(&MTouchEvent, MTouchInput);
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
  *            PLL_Q                          = 8
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 6
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
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
  RCC_OscInitStruct.PLL.PLLQ = 8;

  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Activate the OverDrive to reach the 200 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
