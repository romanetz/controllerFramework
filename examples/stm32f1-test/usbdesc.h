#ifndef __USBDESC_H__
#define __USBDESC_H__

#include <usb.h>

static const USBDeviceDescriptor usbDeviceDescr = {
	.bLength = sizeof(USBDeviceDescriptor),
	.bDescriptorType = USB_DESCR_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = USB_CLASS_CDC,
	.bDeviceSubClass = USB_SUBCLASS_ACM,
	.bDeviceProtocol = 1,
	.bMaxPacketSize = 8,
	.idVendor = 0x0483,
	.idProduct = 0x5740,
	.bcdDevice = 0x0200,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1
};

static const struct {
	USBConfigDescriptor descr;
	USBInterfaceDescriptor iface0descr;
	uint8_t cdcData[19];
	USBEndpointDescriptor endpoint1descr;
	USBInterfaceDescriptor iface1descr;
	USBEndpointDescriptor endpoint1outDescr;
	USBEndpointDescriptor endpoint1inDescr;
} __attribute__((packed)) usbConfig1 = {
	{
		.bLength = sizeof(USBConfigDescriptor),
		.bDescriptorType = USB_DESCR_CONFIG,
		.wTotalLength = sizeof(usbConfig1),
		.bNumInterfaces = 2,
		.bConfigurationValue = 1,
		.iConfiguration = 0,
		.bmAttributes = USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_REMOTE_WAKEUP,
		.bMaxPower = 100 / 2
	},
	{
		.bLength = sizeof(USBInterfaceDescriptor),
		.bDescriptorType = USB_DESCR_INTERFACE,
		.bInterfaceNumber = 0,
		.bAlternateSetting = 0,
		.bNumEndpoints = 1,
		.bInterfaceClass = USB_CLASS_CDC,
		.bInterfaceSubClass = USB_SUBCLASS_ACM,
		.bInterfaceProtocol = 1,
		.iInterface = 0
	},
	{
		5,           /* sizeof(usbDescrCDC_HeaderFn): length of descriptor in bytes */
		0x24,        /* descriptor type */
		0,           /* header functional descriptor */
		0x10, 0x01,

		4,           /* sizeof(usbDescrCDC_AcmFn): length of descriptor in bytes */
		0x24,        /* descriptor type */
		2,           /* abstract control management functional descriptor */
		0x02,        /* SET_LINE_CODING,    GET_LINE_CODING, SET_CONTROL_LINE_STATE    */

		5,           /* sizeof(usbDescrCDC_UnionFn): length of descriptor in bytes */
		0x24,        /* descriptor type */
		6,           /* union functional descriptor */
		0,           /* CDC_COMM_INTF_ID */
		1,           /* CDC_DATA_INTF_ID */

		5,           /* sizeof(usbDescrCDC_CallMgtFn): length of descriptor in bytes */
		0x24,        /* descriptor type */
		1,           /* call management functional descriptor */
		3,           /* allow management on data interface, handles call management by itself */
		1,           /* CDC_DATA_INTF_ID */
	},
	{
		.bLength = sizeof(USBEndpointDescriptor),
		.bDescriptorType = USB_DESCR_ENDPOINT,
		.bEndpointAddress = 0x01 | USB_ENDPOINT_IN,
		.bmAttributes = USB_ENDPOINT_INTERRUPT,
		.wMaxPacketSize = 8,
		.bInterval = 100
	},
	{
		.bLength = sizeof(USBInterfaceDescriptor),
		.bDescriptorType = USB_DESCR_INTERFACE,
		.bInterfaceNumber = 1,
		.bAlternateSetting = 0,
		.bNumEndpoints = 2,
		.bInterfaceClass = 0x0A,
		.bInterfaceSubClass = 0,
		.bInterfaceProtocol = 0,
		.iInterface = 0
	},
	{
		.bLength = sizeof(USBEndpointDescriptor),
		.bDescriptorType = USB_DESCR_ENDPOINT,
		.bEndpointAddress = 0x02 | USB_ENDPOINT_OUT,
		.bmAttributes = USB_ENDPOINT_BULK,
		.wMaxPacketSize = 64,
		.bInterval = 0
	},
	{
		.bLength = sizeof(USBEndpointDescriptor),
		.bDescriptorType = USB_DESCR_ENDPOINT,
		.bEndpointAddress = 0x02 | USB_ENDPOINT_IN,
		.bmAttributes = USB_ENDPOINT_BULK,
		.wMaxPacketSize = 64,
		.bInterval = 0
	}
};

static const USBConfigDescriptor *usbConfigs[] = {
	&usbConfig1.descr
};

static const USB_STRING(usbString0, 0x409);
static const USB_STRING(usbString1, 'K', 'I', 'V', 'i');
static const USB_STRING(usbString2, 'V', 'i', 'r', 't', 'u', 'a', 'l', ' ', 's', 'e', 'r', 'i', 'a', 'l', ' ', 'p', 'o', 'r', 't');
static const USB_STRING(usbString3, '0');
static const USBStringDescriptor *usbStrings[] = {
	&usbString0.descr,
	&usbString1.descr,
	&usbString2.descr,
	&usbString3.descr
};

#endif
