/** @file

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef  _COMMON_KVM_H_
#define  _COMMON_KVM_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stm32f1xx_hal.h"
#include "hal_uart.h"
#include "usb_device.h"

#define RegBase        (0x40005C00L)
#define FNR            ((__IO unsigned *)(RegBase + 0x48))
#define _GetFNR()      ((uint16_t) *FNR)

#define MAX_CMD_LEN  20
#define MAX_RPT_LEN  9

#define HID_DEV_KEYBOARD   1
#define HID_DEV_MOUSE      2
#define HID_DEV_ALL        0xFF

#if HID_KB_MS_COMB
extern  USBD_HID_IfHandleType *const hid_comb_if;
#define hid_kb_if  hid_comb_if
#define hid_ms_if  hid_comb_if
#else
extern  USBD_HID_IfHandleType *const hid_kb_if;
extern  USBD_HID_IfHandleType *const hid_ms_if;
#endif

void    hex_dump (uint8_t *buf, int len, uint8_t lvl);
uint8_t get_hid_state (uint8_t device);
void    set_hid_state (uint8_t device, uint8_t state);
uint8_t send_hid_report (USBD_HID_IfHandleType *itf, void *data, uint16_t length, int8_t retry, uint8_t interval);
void    send_uart_raw (uint8_t *buf, int len);

#endif