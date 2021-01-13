/** @file

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "common_kvm.h"

static uint8_t hid_state = 0xff;

void hex_dump (uint8_t *buf, int len, uint8_t lvl)
{
	int i;
	int off;

	off = 0;
	for (i = 0; i < len; i++) {
		if ((i & 0xf) == 0x00) {
			dprintf (lvl, "  %04X: ", off);
		}
		dprintf (lvl, "%02X ", buf[i]);
		if ((i & 0xf) == 0x0f) {
			dprintf (lvl, "\n");
			off += 16;
		}
	}
	if (i & 0xf) {
		dprintf (lvl, "\n");
	}
}

void set_hid_state (uint8_t device, uint8_t state)
{
	if (device == HID_DEV_ALL) {
		hid_state = state ? 0xff : 00;
		return;
	}

	if ((device != HID_DEV_KEYBOARD) && (device != HID_DEV_MOUSE)) {
		return;
	}

	if (state) {
		hid_state |=  (1 << device);
	} else {
		hid_state &= ~(1 << device);
	}
}

uint8_t get_hid_state (uint8_t device)
{
	if ((device != HID_DEV_KEYBOARD) && (device != HID_DEV_MOUSE)) {
		return 0;
	}
	return (hid_state & (1 << device)) ? 1 : 0;
}

uint8_t send_hid_report (USBD_HID_IfHandleType *itf, void *data, uint16_t length, int8_t retry, uint8_t interval)
{
	USBD_ReturnType  ret;

	do {
		ret = USBD_HID_ReportIn (itf, data, length);
		if (ret == USBD_E_OK) {
			break;
		}
		HAL_Delay (interval);
	}   while (retry-- > 0);

	if (ret == USBD_E_OK) {
		return length;
	} else {
		return 0;
	}
}

void send_uart_raw (uint8_t *buf, int len)
{
	for (int i = 0; i < len; i++) {
		Hal_uart_fast_tx (3, buf[i]);
	}
}


