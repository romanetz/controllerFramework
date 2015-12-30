#ifndef __USB_H__
#define __USB_H__

#include <usb-standard.h>

#define USB_MAX_EP0_PACKET_SIZE 128

#ifdef USE_CLASSES

typedef struct USBDriverClass USBDriverClass;

typedef enum USBEndpointType {
	USB_ENDPOINT_CONTROL = 0,
	USB_ENDPOINT_ISOCHRONOUS = 1,
	USB_ENDPOINT_BULK = 2,
	USB_ENDPOINT_INTERRUPT = 3
} USBEndpointType;

typedef enum USBDriverState {
	USB_DRIVER_RESET,
	USB_DRIVER_SUSPEND,
	USB_DRIVER_SET_ADDRESS,
	USB_DRIVER_READY
} USBDriverState;

typedef void (*USBEndpointCallback)(USBDriverClass *usb, uint8_t ep, uint8_t setup, void *arg);

typedef void (*USBDriverResetHookFunc)(USBDriverClass *usb, void *arg);
typedef void (*USBDriverSuspendHookFunc)(USBDriverClass *usb, void *arg);
typedef void (*USBDriverResumeHookFunc)(USBDriverClass *usb, void *arg);
typedef void (*USBDriverSetAddressHook)(USBDriverClass *usb, uint8_t addr, void *arg);
typedef void (*USBDriverSetConfigHookFunc)(USBDriverClass *usb, const USBConfigDescriptor *config, void *arg);
typedef uint8_t (*USBDriverSetupHookFunc)(USBDriverClass *usb, USBSetupRequest *rq, uint8_t *data, uint16_t dataLen, void *arg);

typedef struct USBDriverHooks USBDriverHooks;

typedef struct USBDriverHooks {
	USBDriverHooks *next;
	void *arg;
	USBDriverResetHookFunc resetHook;
	USBDriverSuspendHookFunc suspendHook;
	USBDriverResumeHookFunc resumeHook;
	USBDriverSetAddressHook setAddressHook;
	USBDriverSetConfigHookFunc setConfigHook;
	USBDriverSetupHookFunc setupRequestHook;
} USBDriverHooks;

typedef void (*USBDriverClassSetAddressFunc)(USBDriverClass *usb, uint8_t addr);
typedef uint8_t (*USBDriverClassEpSetupFunc)(USBDriverClass *usb, uint8_t ep, USBEndpointType type, uint16_t bufSize, USBEndpointCallback callback, void *arg);
typedef void (*USBDriverClassEpStallFunc)(USBDriverClass *usb, uint8_t ep);
typedef void (*USBDriverClassEpNAKFunc)(USBDriverClass *usb, uint8_t ep);
typedef int (*USBDriverClassEpWriteFunc)(USBDriverClass *usb, uint8_t ep, const void *data, int count);
typedef void (*USBDriverClassEpStartTxFunc)(USBDriverClass *usb, uint8_t ep);
typedef int (*USBDriverClassEpReadFunc)(USBDriverClass *usb, uint8_t ep, void *buffer, int count);
typedef void (*USBDriverClassEpStartRxFunc)(USBDriverClass *usb, uint8_t ep);
typedef uint8_t (*USBDriverClassEpReadyFunc)(USBDriverClass *usb, uint8_t ep);

struct USBDriverClass {
	USBDriverClassSetAddressFunc setAddress;
	USBDriverClassEpSetupFunc epSetup;
	USBDriverClassEpStallFunc epStall;
	USBDriverClassEpNAKFunc epNAK;
	USBDriverClassEpWriteFunc epWrite;
	USBDriverClassEpStartTxFunc epStartTx;
	USBDriverClassEpReadFunc epRead;
	USBDriverClassEpStartRxFunc epStartRx;
	USBDriverClassEpReadyFunc epReady;
	USBDriverState state;
	const USBDeviceDescriptor *deviceDescr;
	const USBConfigDescriptor **configs;
	const USBStringDescriptor **strings;
	int stringCount;
	const USBConfigDescriptor *curConfig;
	uint8_t address;
	USBSetupRequest setupRequest;
	uint8_t *ep0RxBuffer;
	uint16_t ep0RxCount;
	uint16_t ep0RxOffset;
	const uint8_t *ep0TxBuffer;
	uint16_t ep0TxCount;
	uint16_t status;
	USBDriverHooks *hooks;
	uint8_t earlySetAddress; // Must be set to one or zero by the driver implementation
};

#define USB_DRIVER_CLASS(obj) ((USBDriverClass*)(obj))

#define usbDriverSetAddress(usb, addr) USB_DRIVER_CLASS(usb)->setAddress(USB_DRIVER_CLASS(usb), addr)
#define usbDriverEpSetup(usb, ep, type, bufSize, callback, arg) USB_DRIVER_CLASS(usb)->epSetup(USB_DRIVER_CLASS(usb), ep, type, bufSize, callback, arg)
#define usbDriverEpStall(usb, ep) USB_DRIVER_CLASS(usb)->epStall(USB_DRIVER_CLASS(usb), ep)
#define usbDriverEpNAK(usb, ep) USB_DRIVER_CLASS(usb)->epNAK(USB_DRIVER_CLASS(usb), ep)
#define usbDriverEpWrite(usb, ep, data, count) USB_DRIVER_CLASS(usb)->epWrite(USB_DRIVER_CLASS(usb), ep, data, count)
#define usbDriverEpStartTx(usb, ep) USB_DRIVER_CLASS(usb)->epStartTx(USB_DRIVER_CLASS(usb), ep)
#define usbDriverEpRead(usb, ep, buffer, count) USB_DRIVER_CLASS(usb)->epRead(USB_DRIVER_CLASS(usb), ep, buffer, count)
#define usbDriverEpStartRx(usb, ep) USB_DRIVER_CLASS(usb)->epStartRx(USB_DRIVER_CLASS(usb), ep)
#define usbDriverEpReady(usb, ep) USB_DRIVER_CLASS(usb)->epReady(USB_DRIVER_CLASS(usb), ep)

void usbDriverClassInit(void *obj, const USBDeviceDescriptor *deviceDescr, const USBConfigDescriptor **configs,
	const USBStringDescriptor **strings, int stringCount);
void usbDriverHandleReset(void *obj);
void usbDriverHandleSuspend(void *obj);
void usbDriverHandleResume(void *obj);
void usbDriverRegisterHooks(void *obj, USBDriverHooks *hooks);

void usbDriverEp0Write(USBDriverClass *usb, const void *data, int count);
void usbDriverEpWriteZLP(USBDriverClass *usb, uint8_t ep);
void usbDriverEp0WriteZLP(USBDriverClass *usb);

#endif

#endif
