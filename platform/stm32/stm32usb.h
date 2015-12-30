#ifndef __STM32USB_H__
#define __STM32USB_H__

#include <stm32.h>

#ifdef USB_BASE

#include <usb.h>
#include <stm32usbregs.h>

#ifdef USE_CLASSES

typedef struct STM32USBDriverClass STM32USBDriverClass;

typedef struct STM32USBEpInfo {
	uint16_t txBufSize;
	USBEndpointCallback inCallback;
	void *inCallbackArg;
	USBEndpointCallback outCallback;
	void *outCallbackArg;
} STM32USBEpInfo;

struct STM32USBDriverClass {
	USBDriverClass parent;
	uint16_t pmaFreeOffset;
	STM32USBEpInfo epInfo[STM32USB_MAX_EP_COUNT];
};

#define STM32_USB_DRIVER_CLASS(obj) ((STM32USBDriverClass*)(obj))

STM32USBDriverClass *stm32_usbDriverClassInit(STM32USBDriverClass *usb, const USBDeviceDescriptor *deviceDescr, const USBConfigDescriptor **configs,
	const USBStringDescriptor **strings, int stringCount);

#endif

#endif

#endif
