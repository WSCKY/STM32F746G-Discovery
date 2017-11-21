/**
  ******************************************************************************
  * @file    Project/Src/main.c
  * @author  kyChu
  * @version V1.0.0
  * @date    20-November-2017
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
static void BSP_PWM_Init(void);
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
	/* Configure PWM */
	BSP_PWM_Init();
	/* Initialize the Touch screen */
	BSP_TS_Init(480, 272);
	/* Init the STemWin GUI Library */
	BSP_SDRAM_Init(); /* Initializes the SDRAM device */
	__HAL_RCC_CRC_CLK_ENABLE(); /* Enable the CRC Module */
	GUI_Init();

	GUI_DispStringAt("Starting...", 0, 0);

	GUI_Initialized = 1;

	/* Activate the use of memory device feature */
	WM_SetCreateFlags(WM_CF_MEMDEV);

	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	GUI_CURSOR_Show();

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
  static GUI_PID_STATE TS_State = {0, 0, 0, 0};
  __IO TS_StateTypeDef  ts;
  uint16_t xDiff, yDiff;  

  BSP_TS_GetState((TS_StateTypeDef *)&ts);

  if((ts.touchX[0] >= LCD_GetXSize()) ||(ts.touchY[0] >= LCD_GetYSize()) ) 
  {
    ts.touchX[0] = 0;
    ts.touchY[0] = 0;
    ts.touchDetected =0;
  }

  xDiff = (TS_State.x > ts.touchX[0]) ? (TS_State.x - ts.touchX[0]) : (ts.touchX[0] - TS_State.x);
  yDiff = (TS_State.y > ts.touchY[0]) ? (TS_State.y - ts.touchY[0]) : (ts.touchY[0] - TS_State.y);
  
  
  if((TS_State.Pressed != ts.touchDetected ) ||
     (xDiff > 30 )||
      (yDiff > 30))
  {
    TS_State.Pressed = ts.touchDetected;
    TS_State.Layer = 0;
    if(ts.touchDetected) 
    {
      TS_State.x = ts.touchX[0];
      TS_State.y = ts.touchY[0];
      GUI_TOUCH_StoreStateEx(&TS_State);
    }
    else
    {
      GUI_TOUCH_StoreStateEx(&TS_State);
      TS_State.x = 0;
      TS_State.y = 0;
    }
  }
}

static void BSP_PWM_Init(void)
{
	/* Timer handler declaration */
	TIM_HandleTypeDef    TimHandle;
	/* Timer Output Compare Configuration Structure declaration */
	TIM_OC_InitTypeDef sConfig;
	/* Counter Prescaler value */
	uint32_t uhPrescalerValue_x = 0, uhPrescalerValue_y = 0;

	GPIO_InitTypeDef   GPIO_InitStruct;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIMx, TIMy Peripheral clock enable */
  TIMx_CLK_ENABLE();
	TIMy_CLK_ENABLE();

  /* Enable all GPIO Channels Clock requested */
  TIMx_CHANNEL_GPIO_PORT();
	TIMy_CHANNEL_GPIO_PORT();

  /* Configure PB.04 (D3 in CN4 connector) (TIM3_Channel1), PA.08 (D5 in CN4 connector) (TIM1_Channel1) in output, push-pull, alternate function mode */
  /* Common configuration for all channels */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct.Alternate = TIMx_GPIO_AF_CHANNEL1;
  GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL1;
  HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL1, &GPIO_InitStruct);

	GPIO_InitStruct.Alternate = TIMy_GPIO_AF_CHANNEL1;
  GPIO_InitStruct.Pin = TIMy_GPIO_PIN_CHANNEL1;
  HAL_GPIO_Init(TIMy_GPIO_PORT_CHANNEL1, &GPIO_InitStruct);

	/* Compute the prescaler value to have TIM1 counter clock equal to 1000000 Hz */
	uhPrescalerValue_x = (uint32_t)(SystemCoreClock / 1000000) - 1;
	/* Compute the prescaler value to have TIM3 counter clock equal to 1000000 Hz */
	uhPrescalerValue_y = (uint32_t)((SystemCoreClock / 2) / 1000000) - 1;

  /*##-1- Configure the TIM peripheral #######################################*/
  /* -----------------------------------------------------------------------
  TIM3 Configuration: generate 1 PWM signals with 5% duty cycles.

    In this example TIM3 input clock (TIM3CLK) is set to APB1 clock,
    since APB1 prescaler is equal to 2.
      TIM3CLK = APB1CLK * 2
      APB1CLK = HCLK / 4
      => TIM3CLK = HCLK = SystemCoreClock / 2

    To get TIM3 counter clock at 1 MHz, the prescaler is computed as follows:
       Prescaler = (TIM3CLK / TIM3 counter clock) - 1
       Prescaler = ((SystemCoreClock/2) /1 MHz) - 1

    To get TIM3 output clock at 100 Hz, the period (ARR)) is computed as follows:
       ARR = (TIM3 counter clock / TIM3 output clock) - 1
           = 9999

    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR + 1)* 100 = 5%

    Note:
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f7xx.c file.
     Each time the core clock (HCLK) changes, user had to update SystemCoreClock
     variable value. Otherwise, any configuration based on this variable will be incorrect.
     This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
  ----------------------------------------------------------------------- */

  /* Initialize TIMx peripheral as follows:
       + Prescaler = ((SystemCoreClock/2) / 1000000) - 1
       + Period = (10000 - 1)
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Instance = TIMx;

  TimHandle.Init.Prescaler         = uhPrescalerValue_x;
  TimHandle.Init.Period            = 9999;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;
  if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /*##-2- Configure the PWM channels #########################################*/
  /* Common configuration for all channels */
  sConfig.OCMode       = TIM_OCMODE_PWM1;
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  /* Set the pulse value for channel 1 */
  sConfig.Pulse = 500;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Configuration Error */
    while(1);
  }

  /*##-3- Start PWM signals generation #######################################*/
  /* Start channel 1 */
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
  {
    /* PWM Generation Error */
    while(1);
  }

	/*##-1- Configure the TIM peripheral #######################################*/
	TimHandle.Instance = TIMy;
	TimHandle.Init.Prescaler = uhPrescalerValue_y;
	if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

	/*##-2- Configure the PWM channels #########################################*/
	if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Configuration Error */
    while(1);
  }

	/*##-3- Start PWM signals generation #######################################*/
  /* Start channel 1 */
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
  {
    /* PWM Generation Error */
    while(1);
  }
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
