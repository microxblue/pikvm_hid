/** @file

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef  _USB_DEVICE_H_
#define  _USB_DEVICE_H_

#include <usbd.h>
#include <usbd_cdc.h>
#include <usbd_hid.h>
#include <stdio.h>
#include "usbd.h"

#define  ABSOLUTE_MOUSE_MODE  1

#define  HID_KB_MS_COMB  0

#define  HID_KB_REPORT_RAW_BYTE  8

#if HID_KB_MS_COMB
#define  HID_KB_REPORT_BYTE   (HID_KB_REPORT_RAW_BYTE + 1)
#define  HID_COMB_REPORT_BYTE HID_KB_REPORT_BYTE
#else
#define  HID_KB_REPORT_BYTE   HID_KB_REPORT_RAW_BYTE
#endif

#if ABSOLUTE_MOUSE_MODE
#define  HID_MS_REPORT_BYTE   (HID_KB_REPORT_BYTE - 1)
#else
#define  HID_MS_REPORT_BYTE   (HID_KB_REPORT_BYTE - 4)
#endif

#endif

