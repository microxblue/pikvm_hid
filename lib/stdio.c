/*
 * stdio.c
 *
 *  Created on: Sep 17, 2018
 *      Author: maanu
 */

#include "stdint.h"
#include "stdio.h"

#include "armcpu.h"
#include "hal_uart.h"

#define PRINTF_BUF_LEN  64

#define out(c)  	*bf++ = c

static
void putch (
  char   *ptr,
  char    ch
)
{
	if (ptr) {
		*ptr = ch;
	} else {
		Hal_uart_fast_tx(1, ch);
	}
}

uint32_t putstr(const char* s)
{
    uint32_t c = 0;
    while(*s)
    {
        Hal_uart_fast_tx(1, *s++);
        c++;
    }
    return c;
}

static
int buf_printf ( char *s, int n, const char * fmt, va_list va)
{
	char         ch;
	unsigned int mask;
  unsigned int num;
  char         buf[12];
  char*        bf;
  char*        p;
  char*        sh;
  char*        st;
  char         uc;
  char         zs;

	sh = s;
	st = s + n;
	if (sh == 0)  st = 0;

	while ((ch=*(fmt++))) {
    if (ch!='%') {
			putch((sh < st) ? sh++ : 0, ch);
		}
		else {
			char lz=0;
			char w =1;
      char hx=0;
      unsigned char dgt;
			ch=*(fmt++);
			if (ch=='0') {
				ch=*(fmt++);
				lz=1;
			}
			if (ch>='0' && ch<='9') {
				w=0;
				while (ch>='0' && ch<='9') {
					w=(((w<<2)+w)<<1)+ch-'0';
					ch=*fmt++;
				}
			}
			bf=buf;
			p=bf;
			zs=0;
			switch (ch) {
				case 0:
					goto abort;

        case 'x':
				case 'X' :
          hx = 1;
				case 'u':
				case 'd' :
					num = va_arg(va, unsigned int);
	        uc  = (ch=='X') ? 'A' : 'a';
					if (ch=='d' && (int)num<0) {
						num = -(int)num;
						out('-');
					}
          if (hx)
            mask = 0x10000000;
          else
            mask = 1000000000;
          while (mask) {
            dgt = 0;
            while (num >= mask) {
		          num -= mask;
		          dgt++;
            }
            if (zs || dgt>0) {
              out(dgt+(dgt<10 ? '0' : uc-10));
	            zs=1;
	          }
            mask = hx ? mask >> 4 : mask / 10;
          }
          if (zs == 0) out('0');
					break;
				case 'c' :
					out((char)(va_arg(va, int)));
					break;
				case 's' :
					p=va_arg(va, char*);
					break;
				case '%' :
					out('%');
				default:
					break;
			}
			*bf=0;
			bf=p;
			while (*bf++ && w > 0)
				w--;
			while (w-- > 0)
        putch((sh < st) ? sh++ : 0, lz ? '0' : ' ');
			while ((ch= *p++))
				putch((sh < st) ? sh++ : 0, ch);
		}
	}
	if (sh < st) {
		*sh = 0;
	}

abort:;

	return sh - s;
}

uint32_t snprintf(char* buf, int n, const char* format, ...)
{
  uint32_t ret;
  va_list  args;
  va_start (args, format);
  ret = buf_printf (buf, n, format, args);
  va_end (args);
  return ret;
}

uint32_t dbg_printf(uint8_t level, const char* format, ...)
{
  char printf_buf[PRINTF_BUF_LEN] = {0};

  if (level > DEBUG_LEVEL) {
    return 0;
  }

  va_list args;
  va_start (args, format);
  buf_printf (printf_buf, PRINTF_BUF_LEN, format, args);
  va_end (args);

  return putstr (printf_buf);
}

uint32_t utoa(char* buf, uint32_t val, utoa_t base)
{
    uint32_t c = 0;
    int32_t  idx = 0;
    char     tmp[11];   // It is big enough for store 32 bit int

    do {
        uint32_t t = val % (uint32_t)base;
        if (t >= 10)
        {
            t += 'A' - '0' - 10;
        }
        tmp[idx] = (t + '0');
        val /= base;
        idx++;
    } while(val);

    // reverse
    idx--;
    while (idx >= 0)
    {
        buf[c++] = tmp[idx];
        idx--;
    }

    return c;
}
