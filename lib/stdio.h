/*
 * stdio.h
 *
 *  Created on: Sep 17, 2018
 *      Author: maanu
 */

#ifndef LIB_STDIO_H_
#define LIB_STDIO_H_

#include "stdint.h"
#include "stdarg.h"

typedef enum utoa_t
{
    utoa_dec = 10,
    utoa_hex = 16,
} utoa_t;

// PiKVM has around 10ms polling interval,
// Should limit UART debug message level to avoid CMD timeout
#ifndef  DEBUG_LEVEL
#define  DEBUG_LEVEL       3
#endif

#define  dprintf      dbg_printf

uint32_t puts(const char* s);
uint32_t dbg_printf(uint8_t level, const char* format, ...);
uint32_t snprintf(char* buf, int n, const char* format, ...);
uint32_t utoa(char* buf, uint32_t val, utoa_t base);
int      printf(const char *format, ...);

#endif /* LIB_STDIO_H_ */
