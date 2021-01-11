/** @file

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "stdint.h"
#include "stdbool.h"
#include "stm32f1xx_hal.h"
#include "hal_uart.h"
#include "kernel.h"

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

void Hal_uart_init (void)
{
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	huart3.gState = HAL_UART_STATE_RESET;
	HAL_UART_Init (&huart3);

	__HAL_UART_ENABLE_IT (&huart3, UART_IT_RXNE);

	HAL_NVIC_SetPriority (USART3_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ (USART3_IRQn);

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.gState = HAL_UART_STATE_RESET;
	HAL_UART_Init (&huart1);
}

void Hal_uart_put_char (uint8_t ch)
{
	HAL_UART_Transmit (&huart1, &ch, 1, 1000);
}

uint8_t Hal_uart_get_char (void)
{
	uint8_t ch;
	if (HAL_UART_Receive (&huart1, &ch, 1, 1000) == HAL_OK) {
		return ch;
	} else {
		return 0;
	}
}

uint8_t Hal_uart_fast_rx (uint8_t idx)
{
	UART_HandleTypeDef  *huart;
	if (idx == 1) {
		huart = &huart1;
	} else if (idx == 3) {
		huart = &huart3;
	} else {
		huart = NULL;
	}
	if (huart) {
		return (huart->Instance->DR & 0xff);
	} else {
		return 0;
	}
}

void Hal_uart_fast_tx (uint8_t idx, uint8_t ch)
{
	UART_HandleTypeDef  *huart;
	if (idx == 1) {
		huart = &huart1;
	} else if (idx == 3) {
		huart = &huart3;
	} else {
		huart = NULL;
	}
	if (huart) {
		while (!(huart->Instance->SR & UART_FLAG_TXE));
		huart->Instance->DR = ch;
	}
}

void Hal_uart_isr (void)
{
	static  uint32_t  last_tick = 0;
	static  uint8_t   cmd_cnt   = 0;
	static  uint8_t   cmd_buf[UART_RX_BUF_LEN];
	uint8_t           ch;
	bool              send_frame;
	uint32_t          curr_tick;

	ch = Hal_uart_fast_rx (3);

	curr_tick = HAL_GetTick();
	if (curr_tick >= last_tick) {
		curr_tick -= last_tick;
	} else {
		curr_tick = curr_tick - last_tick + 0xffffffff;
	}

	if (curr_tick > 50) {
		cmd_cnt  = 0;
	}

	if (cmd_cnt >= UART_RX_BUF_LEN) {
		cmd_cnt  = 0;
	}

	cmd_buf[cmd_cnt++] = ch;

	send_frame = false;
	if ((cmd_buf[0] == '@') || (cmd_buf[0] == '#')) {
		if ((ch == '\r') || (ch == '\n')) {
			send_frame = true;
		}
	} else if (cmd_buf[0] == '3') {
		if (cmd_cnt == 8) {
			send_frame = true;
		}
	}

	if (send_frame) {
		if (Kernel_send_msg (KernelMsgQ_DebugCmd, cmd_buf, cmd_cnt)) {
			Kernel_send_events (KernelEventFlag_CmdIn);
		} else {
			Kernel_flush_msg (KernelEventFlag_CmdIn);
		}
		cmd_cnt = 0;
	}

	last_tick = HAL_GetTick ();
}

