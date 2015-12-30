#ifndef __STM32USBOTG_H__
#define __STM32USBOTG_H__

#include <stm32.h>

#ifdef OTG_FS_DEVICE_BASE

#include <usb.h>
#include <stm32usbotgregs.h>

#ifdef USE_CLASSES

typedef struct STM32USBDriverClass STM32USBDriverClass;

typedef struct STM32USBEpInfo {
	uint16_t txBufSize;
	uint16_t rxBufSize;
	USBEndpointCallback inCallback;
	void *inCallbackArg;
	USBEndpointCallback outCallback;
	void *outCallbackArg;
} STM32USBEpInfo;

struct STM32USBDriverClass {
	USBDriverClass parent;
	volatile STM32OTGGlobalRegs *globalRegs;
	volatile STM32OTGPowerRegs *powerRegs;
	volatile STM32OTGDeviceRegs *deviceRegs;
	STM32USBEpInfo epInfo[STM32USB_MAX_EP_COUNT];
	uint16_t fifoTop;
	int rxCount;
};

#define STM32_USB_DRIVER_CLASS(obj) ((STM32USBDriverClass*)(obj))

STM32USBDriverClass *stm32_usbDriverClassInit(STM32USBDriverClass *usb, const USBDeviceDescriptor *deviceDescr, const USBConfigDescriptor **configs,
	const USBStringDescriptor **strings, int stringCount);

#endif

#endif

#endif
