/*
 * HalUart.h
 *
 *  Created on: Sep 8, 2018
 *      Author: maanu
 */

#ifndef HAL_HALUART_H_
#define HAL_HALUART_H_

#define UART_RX_BUF_LEN   32

void    Hal_uart_init(void);
void    Hal_uart_put_char(uint8_t ch);
uint8_t Hal_uart_get_char(void);

uint8_t Hal_uart_fast_rx (uint8_t idx);
void    Hal_uart_fast_tx (uint8_t idx, uint8_t ch);

#endif /* HAL_HALUART_H_ */
