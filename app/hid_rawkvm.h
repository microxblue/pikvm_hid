/** @file

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef  _HID_RAW_KVM_H_
#define  _HID_RAW_KVM_H_

#include "common_kvm.h"

void handle_rawkvm_command (uint8_t *cmd_buf, uint8_t cmd_len);

#endif

