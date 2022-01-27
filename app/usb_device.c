/** @file

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "usb_device.h"

/** @brief USB device configuration */
const USBD_DescriptionType hdev_cfg = {
							   .Vendor = {
								   .Name = "MX",
								   .ID = 0x0483,
							   },
							   .Product = {
								   .Name = "MX HID Emulator",
								   .ID = 0x5740,
								   .Version.bcd = 0x0100,
							   },
							   .Config = {
								   .Name = "MX Keyboard and Mouse config",
								   .MaxCurrent_mA = 100,
								   .RemoteWakeup = 0,
								   .SelfPowered = 0,
							   },
},
						   *const dev_cfg = &hdev_cfg;

#if HID_KB_MS_COMB

#define HID_COMB_REPORT_DESC_SIZE (65 + 54 + ABSOLUTE_MOUSE_MODE)
/* USB Keyboard Descriptor */
__ALIGN_BEGIN static uint8_t HID_COMB_ReportDesc[HID_COMB_REPORT_DESC_SIZE] __ALIGN_END = {
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	0x09, 0x06, // USAGE (Keyboard)
	0xa1, 0x01, // COLLECTION (Application)
	0x85, 0x01, //   REPORT_ID (1)
	0x05, 0x07, //   USAGE_PAGE (Keyboard)(Key Codes)
	0x19, 0xe0, //   USAGE_MINIMUM (Keyboard LeftControl)(224)
	0x29, 0xe7, //   USAGE_MAXIMUM (Keyboard Right GUI)(231)
	0x15, 0x00, //   LOGICAL_MINIMUM (0)
	0x25, 0x01, //   LOGICAL_MAXIMUM (1)
	0x75, 0x01, //   REPORT_SIZE (1)
	0x95, 0x08, //   REPORT_COUNT (8)
	0x81, 0x02, //   INPUT (Data,Var,Abs) ; Modifier byte
	0x95, 0x01, //   REPORT_COUNT (1)
	0x75, 0x08, //   REPORT_SIZE (8)
	0x81, 0x03, //   INPUT (Cnst,Var,Abs) ; Reserved byte
	0x95, 0x05, //   REPORT_COUNT (5)
	0x75, 0x01, //   REPORT_SIZE (1)
	0x05, 0x08, //   USAGE_PAGE (LEDs)
	0x19, 0x01, //   USAGE_MINIMUM (Num Lock)
	0x29, 0x05, //   USAGE_MAXIMUM (Kana)
	0x91, 0x02, //   OUTPUT (Data,Var,Abs) ; LED report
	0x95, 0x01, //   REPORT_COUNT (1)
	0x75, 0x03, //   REPORT_SIZE (3)
	0x91, 0x03, //   OUTPUT (Cnst,Var,Abs) ; LED report padding
	0x95, 0x06, //   REPORT_COUNT (6)
	0x75, 0x08, //   REPORT_SIZE (8)
	0x15, 0x00, //   LOGICAL_MINIMUM (0)
	0x25, 0x65, //   LOGICAL_MAXIMUM (101)
	0x05, 0x07, //   USAGE_PAGE (Keyboard)(Key Codes)
	0x19, 0x00, //   USAGE_MINIMUM (Reserved (no event indicated))(0)
	0x29, 0x65, //   USAGE_MAXIMUM (Keyboard Application)(101)
	0x81, 0x00, //   INPUT (Data,Ary,Abs)
	0xc0,		// END_COLLECTION

	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	0x09, 0x02, // USAGE (Mouse)
	0xa1, 0x01, // COLLECTION (Application)
	0x85, 0x02, //   REPORT_ID (2)
	0x09, 0x01, //   USAGE (Pointer)
	0xa1, 0x00, //   COLLECTION (Physical)
	0x05, 0x09, //     USAGE_PAGE (Button)
	0x19, 0x01, //     USAGE_MINIMUM (Button 1)
	0x29, 0x03, //     USAGE_MAXIMUM (Button 3)
	0x15, 0x00, //     LOGICAL_MINIMUM (0)
	0x25, 0x01, //     LOGICAL_MAXIMUM (1)
	0x95, 0x03, //     REPORT_COUNT (3)
	0x75, 0x01, //     REPORT_SIZE (1)
	0x81, 0x02, //     INPUT (Data,Var,Abs)
	0x95, 0x01, //     REPORT_COUNT (1)
	0x75, 0x05, //     REPORT_SIZE (5)
	0x81, 0x01, //     INPUT (Cnst)
	0x05, 0x01, //     USAGE_PAGE (Generic Desktop)
	0x09, 0x30, //     USAGE (X)
	0x09, 0x31, //     USAGE (Y)
	0x09, 0x38, //     USAGE (Wheel)
#ifdef ABSOLUTE_MOUSE_MODE
	0x15, 0x00,		 //     LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x7f //     LOGICAL_MAXIMUM (32767)
	0x75,
	0x10,		//     REPORT_SIZE  (16)
	0x95, 0x03, //     REPORT_COUNT (3)
	0x81, 0x02, //     INPUT (Data,Var,Abs)      <<<< This allows us to moveTo absolute positions
#else
	0x15, 0x81, //     LOGICAL_MINIMUM (-127)
	0x25, 0x7f, //     LOGICAL_MAXIMUM (127)
	0x75, 0x08, //     REPORT_SIZE (8)
	0x95, 0x03, //     REPORT_COUNT (3)
	0x81, 0x06, //     INPUT (Data,Var,Rel)
#endif
	0xc0, //   END_COLLECTION
	0xc0  // END_COLLECTION

};

#else

#define HID_KB_REPORT_DESC_SIZE (63)
__ALIGN_BEGIN static uint8_t HID_KB_ReportDesc[HID_KB_REPORT_DESC_SIZE] __ALIGN_END = {
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	0x09, 0x06, // USAGE (Keyboard)
	0xa1, 0x01, // COLLECTION (Application)
	0x05, 0x07, //   USAGE_PAGE (Keyboard)(Key Codes)
	0x19, 0xe0, //   USAGE_MINIMUM (Keyboard LeftControl)(224)
	0x29, 0xe7, //   USAGE_MAXIMUM (Keyboard Right GUI)(231)
	0x15, 0x00, //   LOGICAL_MINIMUM (0)
	0x25, 0x01, //   LOGICAL_MAXIMUM (1)
	0x75, 0x01, //   REPORT_SIZE (1)
	0x95, 0x08, //   REPORT_COUNT (8)
	0x81, 0x02, //   INPUT (Data,Var,Abs) ; Modifier byte
	0x95, 0x01, //   REPORT_COUNT (1)
	0x75, 0x08, //   REPORT_SIZE (8)
	0x81, 0x03, //   INPUT (Cnst,Var,Abs) ; Reserved byte
	0x95, 0x05, //   REPORT_COUNT (5)
	0x75, 0x01, //   REPORT_SIZE (1)
	0x05, 0x08, //   USAGE_PAGE (LEDs)
	0x19, 0x01, //   USAGE_MINIMUM (Num Lock)
	0x29, 0x05, //   USAGE_MAXIMUM (Kana)
	0x91, 0x02, //   OUTPUT (Data,Var,Abs) ; LED report
	0x95, 0x01, //   REPORT_COUNT (1)
	0x75, 0x03, //   REPORT_SIZE (3)
	0x91, 0x03, //   OUTPUT (Cnst,Var,Abs) ; LED report padding
	0x95, 0x06, //   REPORT_COUNT (6)
	0x75, 0x08, //   REPORT_SIZE (8)
	0x15, 0x00, //   LOGICAL_MINIMUM (0)
	0x25, 0x65, //   LOGICAL_MAXIMUM (101)
	0x05, 0x07, //   USAGE_PAGE (Keyboard)(Key Codes)
	0x19, 0x00, //   USAGE_MINIMUM (Reserved (no event indicated))(0)
	0x29, 0x65, //   USAGE_MAXIMUM (Keyboard Application)(101)
	0x81, 0x00, //   INPUT (Data,Ary,Abs)
	0xc0,		// END_COLLECTION
};

#define HID_MS_REPORT_DESC_SIZE (52 + ABSOLUTE_MOUSE_MODE)
__ALIGN_BEGIN static uint8_t HID_MS_ReportDesc[HID_MS_REPORT_DESC_SIZE] __ALIGN_END = {
	0x05, 0x01,		  // USAGE_PAGE (Generic Desktop)
	0x09, 0x02,		  // USAGE (Mouse)
	0xa1, 0x01,		  // COLLECTION (Application)
	0x09, 0x01,		  //   USAGE (Pointer)
	0xa1, 0x00,		  //   COLLECTION (Physical)
	0x05, 0x09,		  //     USAGE_PAGE (Button)
	0x19, 0x01,		  //     USAGE_MINIMUM (Button 1)
	0x29, 0x03,		  //     USAGE_MAXIMUM (Button 3)
	0x15, 0x00,		  //     LOGICAL_MINIMUM (0)
	0x25, 0x01,		  //     LOGICAL_MAXIMUM (1)
	0x95, 0x03,		  //     REPORT_COUNT (3)
	0x75, 0x01,		  //     REPORT_SIZE (1)
	0x81, 0x02,		  //     INPUT (Data,Var,Abs)
	0x95, 0x01,		  //     REPORT_COUNT (1)
	0x75, 0x05,		  //     REPORT_SIZE (5)
	0x81, 0x01,		  //     INPUT (Cnst)
	0x05, 0x01,		  //     USAGE_PAGE (Generic Desktop)
	0x09, 0x30,		  //     USAGE (X)
	0x09, 0x31,		  //     USAGE (Y)
	0x09, 0x38,		  //     USAGE (W)
#ifdef ABSOLUTE_MOUSE_MODE
	0x15, 0x00,		  //     LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x7f, //     LOGICAL_MAXIMUM (32767)
	0x75, 0x10,		  //     REPORT_SIZE  (16)
	0x95, 0x03,		  //     REPORT_COUNT (3)
	0x81, 0x02,		  //     INPUT (Data,Var,Abs)
#else
	0x15, 0x81, //     LOGICAL_MINIMUM (-127)
	0x25, 0x7f, //     LOGICAL_MAXIMUM (127)
	0x75, 0x08, //     REPORT_SIZE (8)
	0x95, 0x03, //     REPORT_COUNT (3)
	0x81, 0x06, //     INPUT (Data,Var,Rel)
#endif
	0xc0,			  //   END_COLLECTION
	0xc0			  // END_COLLECTION
};
#endif

#if HID_KB_MS_COMB

/** @brief HID report configuration structure */
static USBD_HID_ReportConfigType hid_comb_report = {
	.Desc = HID_COMB_ReportDesc,
	.DescLength = HID_COMB_REPORT_DESC_SIZE,
	.Input.Interval_ms = 20,
	.Input.MaxSize = HID_COMB_REPORT_BYTE,
	.DevType = 1,
};

static const USBD_HID_AppType hid_comb_app = {
	.Name = "KB and MS Emulator",
	.Report = &hid_comb_report,
};

USBD_HID_IfHandleType h_hid_comb_if = {
						  .App = &hid_comb_app,
						  .Base.AltCount = 1,
},
					  *const hid_comb_if = &h_hid_comb_if;

#else

/** @brief HID report configuration structure */
static USBD_HID_ReportConfigType hid_kb_report = {
	.Desc = HID_KB_ReportDesc,
	.DescLength = HID_KB_REPORT_DESC_SIZE,
	.Input.Interval_ms = 20,
	.Input.MaxSize = HID_KB_REPORT_BYTE,
	.DevType = 1,
};

static const USBD_HID_AppType hid_kb_app = {
	.Name = "KB Emulator",
	.Report = &hid_kb_report,
};

USBD_HID_IfHandleType h_hid_kb_if = {
						  .App = &hid_kb_app,
						  .Base.AltCount = 1,
},
					  *const hid_kb_if = &h_hid_kb_if;

static USBD_HID_ReportConfigType hid_ms_report = {
	.Desc = HID_MS_ReportDesc,
	.DescLength = HID_MS_REPORT_DESC_SIZE,
	.Input.Interval_ms = 20,
	.Input.MaxSize = HID_MS_REPORT_BYTE,
	.DevType = 2,
};

static const USBD_HID_AppType hid_ms_app = {
	.Name = "MS Emulator",
	.Report = &hid_ms_report,
};

USBD_HID_IfHandleType h_hid_ms_if = {
						  .App = &hid_ms_app,
						  .Base.AltCount = 1,
},
					  *const hid_ms_if = &h_hid_ms_if;

#endif

USBD_HandleType h_usb_device, *const USB_Device = &h_usb_device;

void usb_connect_ctrl(int connect)
{
	GPIO_PinState state;
	if (connect == ENABLE)
	{
		state = GPIO_PIN_RESET;
	}
	else
	{
		state = GPIO_PIN_SET;
	}
	HAL_GPIO_WritePin(HAL_USB_DISCONNECT, HAL_USB_DISCONNECT_PIN, state);
}

void usb_device_init(void)
{
	/* Hook connect */
	USB_Device->Callbacks.ConnectCtrl = usb_connect_ctrl;

	/* Mount the interfaces to the device */
#if HID_KB_MS_COMB
	/* All fields of Config have to be properly set up */
	hid_comb_if->Config.InEpNum = 0x87;
	USBD_HID_MountInterface(hid_comb_if, USB_Device);
#else
	/* All fields of Config have to be properly set up */
	hid_kb_if->Config.InEpNum = 0x87;
	hid_ms_if->Config.InEpNum = 0x85;
	USBD_HID_MountInterface(hid_kb_if, USB_Device);
	USBD_HID_MountInterface(hid_ms_if, USB_Device);
#endif

	/* Initialize the device */
	USBD_Init(USB_Device, dev_cfg);

	/* The device connection can be made */
	USBD_Connect(USB_Device);
}
