/** @file

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "hid_pikvm.h"
#include "usb_hid_keys.h"

static
uint8_t key_map_usb (uint8_t code)
{
  switch (code) {
    case  1: return KEY_A;
    case  2: return KEY_B;
    case  3: return KEY_C;
    case  4: return KEY_D;
    case  5: return KEY_E;
    case  6: return KEY_F;
    case  7: return KEY_G;
    case  8: return KEY_H;
    case  9: return KEY_I;
    case 10: return KEY_J;
    case 11: return KEY_K;
    case 12: return KEY_L;
    case 13: return KEY_M;
    case 14: return KEY_N;
    case 15: return KEY_O;
    case 16: return KEY_P;
    case 17: return KEY_Q;
    case 18: return KEY_R;
    case 19: return KEY_S;
    case 20: return KEY_T;
    case 21: return KEY_U;
    case 22: return KEY_V;
    case 23: return KEY_W;
    case 24: return KEY_X;
    case 25: return KEY_Y;
    case 26: return KEY_Z;
    case 27: return KEY_1;
    case 28: return KEY_2;
    case 29: return KEY_3;
    case 30: return KEY_4;
    case 31: return KEY_5;
    case 32: return KEY_6;
    case 33: return KEY_7;
    case 34: return KEY_8;
    case 35: return KEY_9;
    case 36: return KEY_0;
    case 37: return KEY_ENTER;
    case 38: return KEY_ESC;
    case 39: return KEY_BACKSPACE;
    case 40: return KEY_TAB;
    case 41: return KEY_SPACE;
    case 42: return KEY_MINUS;
    case 43: return KEY_EQUAL;
    case 44: return KEY_LEFT_BRACE;
    case 45: return KEY_RIGHT_BRACE;
    case 46: return KEY_BACKSLASH;
    case 47: return KEY_SEMICOLON;
    case 48: return KEY_QUOTE;
    case 49: return KEY_TILDE;
    case 50: return KEY_COMMA;
    case 51: return KEY_PERIOD;
    case 52: return KEY_SLASH;
    case 53: return KEY_CAPS_LOCK;
    case 54: return KEY_F1;
    case 55: return KEY_F2;
    case 56: return KEY_F3;
    case 57: return KEY_F4;
    case 58: return KEY_F5;
    case 59: return KEY_F6;
    case 60: return KEY_F7;
    case 61: return KEY_F8;
    case 62: return KEY_F9;
    case 63: return KEY_F10;
    case 64: return KEY_F11;
    case 65: return KEY_F12;
    case 66: return KEY_PRINT;
    case 67: return KEY_INSERT;
    case 68: return KEY_HOME;
    case 69: return KEY_PAGE_UP;
    case 70: return KEY_DELETE;
    case 71: return KEY_END;
    case 72: return KEY_PAGE_DOWN;
    case 73: return KEY_RIGHT_ARROW;
    case 74: return KEY_LEFT_ARROW;
    case 75: return KEY_DOWN_ARROW;
    case 76: return KEY_UP_ARROW;
    case 77: return KEY_LEFT_CTRL;
    case 78: return KEY_LEFT_SHIFT;
    case 79: return KEY_LEFT_ALT;
    case 80: return KEY_LEFT_GUI;
    case 81: return KEY_RIGHT_CTRL;
    case 82: return KEY_RIGHT_SHIFT;
    case 83: return KEY_RIGHT_ALT;
    case 84: return KEY_RIGHT_GUI;
    case 85: return KEY_PAUSE;
    case 86: return KEY_SCROLL_LOCK;
    case 87: return KEY_NUM_LOCK;
    case 88: return KEY_MENU;
    case 89: return KEYPAD_DIVIDE;
    case 90: return KEYPAD_MULTIPLY;
    case 91: return KEYPAD_SUBTRACT;
    case 92: return KEYPAD_ADD;
    case 93: return KEYPAD_ENTER;
    case 94: return KEYPAD_1;
    case 95: return KEYPAD_2;
    case 96: return KEYPAD_3;
    case 97: return KEYPAD_4;
    case 98: return KEYPAD_5;
    case 99: return KEYPAD_6;
    case 100: return KEYPAD_7;
    case 101: return KEYPAD_8;
    case 102: return KEYPAD_9;
    case 103: return KEYPAD_0;
    case 104: return KEYPAD_DOT;
    case 105: return KEY_POWER;
    case 106: return KEY_NON_US;
    default: return KEY_ERROR_UNDEFINED;
  }
}

uint16_t crc16 (const uint8_t *buffer, unsigned length)
{
  const uint16_t polinom = 0xA001;
  uint16_t crc = 0xFFFF;
  for (unsigned byte_count = 0; byte_count < length; ++byte_count) {
    crc = crc ^ buffer[byte_count];
    for (unsigned bit_count = 0; bit_count < 8; ++bit_count) {
      if ((crc & 0x0001) == 0) {
        crc = crc >> 1;
      } else {
        crc = crc >> 1;
        crc = crc ^ polinom;
      }
    }
  }
  return crc;
}


static
void send_uart_rsp (uint8_t code)
{
  static uint8_t prev_code = RESP_NONE;
  if (code == 0) {
    code = prev_code; // Repeat the last code
  } else {
    prev_code = code;
  }

  uint8_t response[8] = {0};
  response[0] = MAGIC_RESP;
  if (code & PONG_OK) {
    response[1]  = PONG_OK;
    response[1] |= get_hid_state (HID_DEV_KEYBOARD) ? 0 : PONG_KEYBOARD_OFFLINE;
    response[1] |= 0; // getLedsAs(PROTO::PONG::CAPS, PROTO::PONG::SCROLL, PROTO::PONG::NUM);
    response[2] |= OUTPUTS_KEYBOARD_USB;
    response[1] |= get_hid_state (HID_DEV_MOUSE) ? 0 : PONG_MOUSE_OFFLINE;
    response[2] |= OUTPUTS_MOUSE_USB_ABS;
    response[3] |= FEATURES_HAS_USB;
  } else {
    response[1] = code;
  }

  uint16_t  crc;
  crc = crc16 (response, 6);
  response[6] = crc >> 8;
  response[7] = crc & 0xff;
  send_uart_raw (response, 8);

  dprintf (4, "KVM TX:\n");
  hex_dump (response, 8, 4);
}


void handle_pikvm_command (uint8_t *cmd_buf, uint8_t cmd_len)
{
  static  uint8_t  modifier = 0;
  static  uint16_t last_crc = 0;
  static  uint16_t posx     = 0;
  static  uint16_t posy     = 0;
  static  uint8_t  button   = 0;

  uint8_t  hid_rpt[MAX_RPT_LEN];
  uint8_t  rpt_len;
  uint8_t  resp;
  uint8_t  sent;
  bool     skip;
  int16_t  x;
  int16_t  y;

  resp = RESP_NONE;

  if ((cmd_buf[0] != MAGIC) || (cmd_len != 8)) {
    dprintf (4, "Header Error\n");
    resp = RESP_INVALID_ERROR;
  }

  uint16_t crc = crc16 (cmd_buf, 6);
  if (((crc & 0xFF) != cmd_buf[7]) || ((crc >> 8) != cmd_buf[6])) {
    dprintf (4, "CRC Error\n");
    resp = RESP_CRC_ERROR;
  }

  if (resp == RESP_NONE) {
    switch (cmd_buf[1]) {
    case CMD_PING:
      resp = PONG_OK;
      break;
    case CMD_CLEAR_HID:
      resp = PONG_OK;
      break;
    case CMD_REPEAT:
      resp = 0;
      break;

    case CMD_BUTTON:
    case CMD_MOVE:
      rpt_len   = HID_MS_REPORT_BYTE;
      hid_rpt[0] = 2;
      if (cmd_buf[1] == CMD_MOVE) {
        x = (int16_t)((cmd_buf[2] << 8) + cmd_buf[3]);
        y = (int16_t)((cmd_buf[4] << 8) + cmd_buf[5]);
        posx = (x + 32768) / 2;
        posy = (y + 32768) / 2;
      } else {
        button = 0;
        if (cmd_buf[2] & 0x08) {
          button |=  1;
        }
        if (cmd_buf[2] & 0x04) {
          button |=  2;
        }
        if (cmd_buf[2] & 0x02) {
          button |=  4;
        }
      }

      hid_rpt[1] = button;
      hid_rpt[2] = posx & 0xff;
      hid_rpt[3] = posx >> 8;
      hid_rpt[4] = posy & 0xff;
      hid_rpt[5] = posy >> 8;

      sent = send_hid_report (hid_ms_if, hid_rpt + (1 - HID_KB_MS_COMB), rpt_len, 3, 10);
      if (sent == rpt_len) {
        set_hid_state (HID_DEV_MOUSE, 1);
      } else {
        set_hid_state (HID_DEV_MOUSE, 0);
      }
      resp = PONG_OK;
      break;

    case CMD_KEY:
      rpt_len = HID_KB_REPORT_BYTE;
      memset (hid_rpt, 0, sizeof (hid_rpt));
      skip       = true;
      hid_rpt[0] = 1;
      hid_rpt[3] = key_map_usb (cmd_buf[2]);
      if (cmd_buf[3]) {
        // key press
        hid_rpt[1] = modifier;
        if ((hid_rpt[3] >= KEY_LEFT_CTRL) && (hid_rpt[3] <= KEY_RIGHT_WINDOWS)) {
          modifier |= (1 << (hid_rpt[3] - KEY_LEFT_CTRL));
        } else {
          if (hid_rpt[3] != KEY_ERROR_UNDEFINED) {
            skip = false;
          }
        }
      } else {
        // key release
        if ((hid_rpt[3] >= KEY_LEFT_CTRL) && (hid_rpt[3] <= KEY_RIGHT_WINDOWS)) {
          modifier &= ~(1 << (hid_rpt[3] - KEY_LEFT_CTRL));
        }
        hid_rpt[1] = 0;
        hid_rpt[3] = 0;
        skip = false;
      }

      if (!skip) {
        crc = crc16 (hid_rpt + (1 - HID_KB_MS_COMB), rpt_len);
        if (last_crc == crc) {
          sent = rpt_len;
        } else {
          sent = send_hid_report (hid_kb_if, hid_rpt + (1 - HID_KB_MS_COMB), rpt_len, 3, 10);
          last_crc = (sent == rpt_len) ? crc : 0;
          if (sent == rpt_len) {
            set_hid_state (HID_DEV_KEYBOARD, 1);
          } else {
            set_hid_state (HID_DEV_KEYBOARD, 0);
          }
        }
      } else {
        sent = rpt_len;
      }
      resp = PONG_OK;
      break;

    default:
      resp = RESP_INVALID_ERROR;
      break;
    }
  }

  send_uart_rsp (resp);
}

