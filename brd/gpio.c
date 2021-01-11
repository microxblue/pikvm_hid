/** @file

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "stm32f1xx_hal.h"

void usb_disconnect (void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Disconnect USB */
  HAL_GPIO_WritePin (HAL_USB_DISCONNECT, HAL_USB_DISCONNECT_PIN, GPIO_PIN_SET);

  // Workaround for Blue-Pill (No USB disconnect control pin)
  GPIO_InitStruct.Pin   = GPIO_PIN_12;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin (GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);

}

void usb_connect (int ms)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  HAL_Delay (ms);

  GPIO_InitStruct.Pin   = GPIO_PIN_12;
  GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Hal_gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

  /* USB connect pin */
  GPIO_InitStruct.Pin   = GPIO_PIN_2;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  // Disconnect USB
  usb_disconnect ();
}
