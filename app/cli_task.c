/** @file

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "stdio.h"
#include "kernel.h"
#include "hid_pikvm.h"
#include "hid_rawkvm.h"

static
void handle_command ()
{
	static   uint16_t last_fnr = 0;
	uint8_t  cmd_buf[MAX_CMD_LEN];
	bool     valid;
	uint16_t fnr;
	uint32_t tick;
	uint8_t  ch;
	uint8_t  i;

	valid = false;
	for (i = 0 ; i < MAX_CMD_LEN; i++)  {
		if (Kernel_recv_msg (KernelMsgQ_DebugCmd, &ch, 1) == 0) {
			valid = true;
			cmd_buf[i]  = 0;
			break;
		} else if  (((cmd_buf[0] != MAGIC)) && ((ch == '\r') || (ch == '\n'))) {
			valid = true;
			cmd_buf[i]  = 0;
			break;
		}   else {
			cmd_buf[i]  = ch;
		}
	}

	if (valid) {
		fnr = _GetFNR();
		if (!(fnr & USB_FNR_LCK)) {
			set_hid_state (HID_DEV_ALL, 0);
		} else if (!(last_fnr & USB_FNR_LCK)) {
			set_hid_state (HID_DEV_ALL, 1);
		}
		last_fnr = fnr;

		tick = HAL_GetTick();
		dprintf (4, "CMD RX: (%8d)\n", tick);
		hex_dump (cmd_buf, i, 4);

		if (cmd_buf[0] == MAGIC) {
			handle_pikvm_command (cmd_buf, i);
		} else {
			handle_rawkvm_command (cmd_buf, i);
		}

		fnr = _GetFNR();
		dprintf (4, "DNE --: (%8d)\n\n", HAL_GetTick() - tick);
	}
}

void cli_task (void)
{
	KernelEventFlag_t handle_event;

	dprintf (1, "Ready\n");
	while (true) {
		handle_event = Kernel_wait_events (KernelEventFlag_CmdIn);
		if (handle_event == KernelEventFlag_CmdIn) {
			handle_command ();
		}
		Kernel_yield();
	}
}
