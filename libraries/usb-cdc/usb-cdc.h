#ifndef __USB_CDC_H__
#define __USB_CDC_H__

#include <usb.h>
#include <iostream.h>
#include <queue.h>

#ifdef USE_CLASSES

typedef struct USBCDCClass USBCDCClass;

struct USBCDCClass {
	IOStreamClass parent;
	USBDriverClass *usb;
	QueueClass txQueue;
	QueueClass rxQueue;
	uint16_t bulkOutChunkSize;
	uint16_t bulkInChunkSize;
	uint16_t intrInChunkSize;
	uint16_t lastChunkSize;
	uint8_t bulkOutEp;
	uint8_t bulkInEp;
	uint8_t intrInEp;
	uint8_t lineCoding[7];
};

#define USB_CDC_CLASS(obj) ((USBCDCClass*)(obj))

USBCDCClass *usbCdcClassInit(USBCDCClass *usbCdc, USBDriverClass *usb, int txBufferSize, int rxBufferSize,
	uint8_t bulkOutEp, uint16_t bulkOutChunkSize, uint8_t bulkInEp, uint16_t bulkInChunkSize, uint8_t intrInEp, uint16_t intrInChunkSize);

#endif

#endif
