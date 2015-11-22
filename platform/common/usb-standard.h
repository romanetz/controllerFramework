#ifndef __USB_STANDARD_H__
#define __USB_STANDARD_H__

#include <stdint.h>

struct USBDeviceRequest {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
} __attribute__((packed));

#define USB_REQ_DIR_MASK 0x80
#define USB_REQ_DIR_HOST_TO_DEVICE 0x00
#define USB_REQ_DIR_DEVICE_TO_HOST 0x80

#define USB_REQ_TYPE_MASK 0x60
#define USB_REQ_TYPE_STANDARD 0
#define USB_REQ_TYPE_CLASS 0x20
#define USB_REQ_TYPE_VENDOR 0x40

#define USB_REQ_RECIPIENT_MASK 0x0F
#define USB_REQ_RECIPIENT_DEVICE 0
#define USB_REQ_RECIPIENT_INTERFACE 1
#define USB_REQ_RECIPIENT_ENDPOINT 2
#define USB_REQ_RECIPIENT_OTHER 3

#define USB_REQ_STD_GET_STATUS 0
#define USB_REQ_STD_CLEAR_FEATURE 1
#define USB_REQ_STD_SET_FEATURE 3
#define USB_REQ_STD_SET_ADDRESS 5
#define USB_REQ_STD_GET_DESCRIPTOR 6
#define USB_REQ_STD_SET_DESCRIPTOR 7
#define USB_REQ_STD_GET_CONFIGURATION 8
#define USB_REQ_STD_SET_CONFIGURATION 9

#define USB_DEVICE_STATUS_SELF_POWERED 1
#define USB_DEVICE_STATUS_REMOTE_WAKEUP 2

#define USB_DEVICE_FEATURE_REMOTE_WAKEUP 1
#define USB_DEVICE_FEATURE_TEST_MODE 2

#define USB_ENDPOINT_FEATURE_HALT 0

struct USBDescriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
} __attribute__((packed));

#define USB_DESCR_DEVICE 1
#define USB_DESCR_CONFIG 2
#define USB_DESCR_STRING 3
#define USB_DESCR_INTERFACE 4
#define USB_DESCR_ENDPOINT 5
#define USB_DESCR_HID 0x21
#define USB_DESCR_HID_REPORT 0x22
#define USB_DESCR_HID_PHYS 0x23

struct USBDeviceDescr {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t iManufacturer;
	uint8_t iProduct;
	uint8_t iSerialNumber;
	uint8_t bNumConfigurations;
} __attribute__((packed));

#define USB_CLASS_COMPOSITE 0
#define USB_CLASS_CDC 2
#define USB_CLASS_VENDOR 0xFF

#define USB_SUBCLASS_ACM 2

struct USBEndpointDescr {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;
	uint16_t wMaxPacketSize;
	uint8_t bInterval;
} __attribute__((packed));

#define USB_ENDPOINT_OUT 0
#define USB_ENDPOINT_IN 0x80

struct USBInterfaceDescr {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
} __attribute__((packed));

struct USBConfigDescr {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t wTotalLength;
	uint8_t bNumInterfaces;
	uint8_t bConfigurationValue;
	uint8_t iConfiguration;
	uint8_t bmAttributes;
	uint8_t bMaxPower;
} __attribute__((packed));

#define USB_CONFIG_ATTR_RESERVED 0x80
#define USB_CONFIG_ATTR_SELF_POWERED (1 << 6)
#define USB_CONFIG_ATTR_REMOTE_WAKEUP (1 << 5)

struct USBStringDescr {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bString[];
} __attribute__((packed));

#define USB_STRING(name, ...) \
	struct { \
		USBStringDescr descr; \
		uint16_t data[sizeof((int[]){__VA_ARGS__}) / sizeof(int)]; \
	} __attribute__((packed)) name = { { sizeof(name), USB_DESCR_STRING }, {__VA_ARGS__} };

#endif
