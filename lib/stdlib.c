/*
 * stdlib.c
 *
 *  Created on: Sep 27, 2018
 *      Author: yiman
 */

#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"

#include "hal_timer.h"


void delay(uint32_t ms)
{
    uint32_t goal = Hal_timer_get_1ms_counter() + ms;

    while(goal != Hal_timer_get_1ms_counter());
}

void memclr(void* dst, uint32_t count)
{
    uint8_t* d = (uint8_t*)dst;

    while(count--)
    {
        *d++ = 0;
    }
}

uint32_t memfind(uint8_t* src, uint8_t byte, uint32_t count)
{
	for(uint32_t i = 0 ; i < count ; i++)
	{
		if (*src++ == byte)
		{
			return i;
		}
	}
	return (count + 1);
}

bool memncmp(uint8_t* m1, uint8_t* m2, uint32_t count)
{
	while(count--)
	{
		if (*m1++ != *m2++)
		{
			return false;
		}
	}

	return true;
}

void memncpy(uint8_t* dst, uint8_t* src, uint32_t count)
{
	while(count--)
	{
		*dst++ = *src++;
	}
}

uint32_t htou(char* ascii, uint32_t count)
{
	uint32_t ret = 0;
	while(count--)
	{
		uint32_t digit = 0;
		if ('0' <= *ascii && *ascii <= '9')
		{
			digit = *ascii - '0';
		}
		else if ('a' <= *ascii && *ascii <= 'f')
		{
			digit = *ascii - 'a' + 10;
		}
		else if ('A' <= *ascii && *ascii <= 'F')
		{
			digit = *ascii - 'A' + 10;
		}
		else
		{
			return 0;
		}
		ascii++;
		ret += digit * (1 << (count*4));
	}

	return ret;
}

void halt(char* filename, uint32_t line)
{
	dprintf(1, "HALT [%s] at (%u)\n", filename, line);
	while(1);
}
