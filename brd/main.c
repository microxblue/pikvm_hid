/** @file

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "stm32f1xx_hal.h"
#include "hal_interrupt.h"

#include "hal_gpio.h"
#include "hal_uart.h"

#include "kernel.h"
#include "memory_map.h"

#include "usbd.h"

#define SYSTEM_US_TICKS (SystemCoreClock / 1000000) // cycles per microsecond

extern void usb_device_init (void);
extern void usb_connect (int ms);

/** System Clock Configuration
*/
static void SystemClock_Config (void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  /**Initializes the CPU, AHB and APB busses clocks
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK) {
    halt (__FILE__, __LINE__);
  }

  /**Initializes the CPU, AHB and APB busses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
    halt (__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig (&PeriphClkInit) != HAL_OK) {
    halt (__FILE__, __LINE__);
  }

  /**Configure the Systick interrupt time
   */
  HAL_SYSTICK_Config (HAL_RCC_GetHCLKFreq() / 1000);

  /**Configure the Systick
   */
  HAL_SYSTICK_CLKSourceConfig (SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority (SysTick_IRQn, 0, 0);
}

static void Kernel_Init (void)
{
  extern void cli_task();

  uint32_t taskId;

  Kernel_task_init();

  taskId = Kernel_task_create (cli_task);
  if (NOT_ENOUGH_TASK_NUM == taskId)  {
    dprintf (1, "Failed to create task: cli_task\n");
  }
}

int main (void)
{
  HAL_Init();

  SystemClock_Config();

  Hal_interrupt_init ();

  Hal_gpio_init();

  Hal_uart_init();

  dprintf (1, "BootLoader\n");

  usb_connect (10);

  usb_device_init ();

  Kernel_Init();

  Kernel_start();

  while (1) { }
}
