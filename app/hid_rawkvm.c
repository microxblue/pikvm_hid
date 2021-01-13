/** @file

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "hid_rawkvm.h"
#include "kernel.h"

void handle_rawkvm_command (uint8_t *cmd_buf, uint8_t cmd_len)
{
	uint8_t  resp[8];
	uint8_t  hid_rpt[MAX_RPT_LEN];
	uint8_t  rpt_len;
	uint8_t  rpt_id;
	uint8_t  error;
	uint8_t  sent;

	error = 0;

	if (cmd_len == 0) {
		error = 1;
	}

	if (cmd_buf[0] == '@') {
		// KB
		rpt_len   = HID_KB_REPORT_BYTE;
		rpt_id    = 1;
	} else if (cmd_buf[0] == '#') {
		// MS
		rpt_len   = HID_MS_REPORT_BYTE;
		rpt_id    = 2;
	} else {
		error = 2;
	}

	if (error == 0) {
		cmd_buf++;
		cmd_len--;
		if (cmd_len > rpt_len) {
			cmd_len = rpt_len;
		}

		memset (hid_rpt, 0, sizeof (hid_rpt));
		hid_rpt[0] = rpt_id;
		memcpy (hid_rpt + 1, cmd_buf, cmd_len);

		hex_dump (hid_rpt + (1 - HID_KB_MS_COMB), rpt_len, 6);
		sent = send_hid_report ( (rpt_id == 1) ? hid_kb_if : hid_ms_if, hid_rpt + (1 - HID_KB_MS_COMB), rpt_len, 3, 10);
		if (sent != rpt_len) {
			error = 3;
		}
	}

	if (error == 0) {
		memcpy (resp, "$OK\n", 4);
		send_uart_raw (resp, 4);
	}   else {
		memcpy (resp, "$ERR0\n", 6);
		resp[3] = '0' + error;
		send_uart_raw (resp, 6);
	}

}
