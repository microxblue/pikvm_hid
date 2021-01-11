/** @file

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef  _HID_PI_KVM_H_
#define  _HID_PI_KVM_H_

#include "common_kvm.h"

#define  MAGIC			            0x33
#define  MAGIC_RESP	            0x34
#define  RESP_NONE              0x24
#define  RESP_CRC_ERROR         0x40
#define  RESP_INVALID_ERROR     0x45
#define  RESP_TIMEOUT_ERROR     0x48
#define  CMD_PING               0x01
#define  CMD_REPEAT             0x02
#define  CMD_SET_KEYBOARD       0x03
#define  CMD_SET_MOUSE          0x04
#define  CMD_CLEAR_HID          0x10
#define  CMD_KEY                0x11
#define  CMD_MOVE               0x12
#define  CMD_BUTTON             0x13
#define  CMD_WHEEL              0x14
#define  CMD_RELATIVE           0x15
#define  PONG_OK                0x80
#define  PONG_CAPS              0x01
#define  PONG_SCROLL            0x02
#define  PONG_NUM               0x04
#define  PONG_KEYBOARD_OFFLINE  0x08
#define  PONG_MOUSE_OFFLINE  		0x10
#define  PONG_RESET_REQUIRED  	0x40

#define  OUTPUTS_KEYBOARD_USB   0x01
#define  OUTPUTS_MOUSE_USB_ABS  0x08
#define  FEATURES_HAS_USB       0x01

void handle_pikvm_command (uint8_t *cmd_buf, uint8_t cmd_len);

#endif
