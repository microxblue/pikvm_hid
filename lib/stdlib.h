/*
 * stdlib.h
 *
 *  Created on: Sep 27, 2018
 *      Author: yiman
 */

#ifndef LIB_STDLIB_H_
#define LIB_STDLIB_H_

#include "stdint.h"
#include "stdbool.h"

void 	 delay(uint32_t ms);
void 	 memclr(void* dst, uint32_t count);
uint32_t memfind(uint8_t* src, uint8_t byte, uint32_t count);
bool	 memncmp(uint8_t* m1, uint8_t* m2, uint32_t count);
void	 memncpy(uint8_t* dst, uint8_t* src, uint32_t count);
uint32_t htou(char* ascii, uint32_t count);

void halt(char* filename, uint32_t line);

#define HALT()	halt(__FILE__, __LINE__);

#endif /* LIB_STDLIB_H_ */
