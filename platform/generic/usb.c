#include <stdlib.h>
#include <usb.h>

#define usbDriverCallHook(usb, hook, ...) do { \
		USBDriverHooks *hooks = usb->hooks; \
		while (hooks) { \
			if (hooks->hook) { \
				hooks->hook(__VA_ARGS__, hooks->arg); \
			} \
			hooks = hooks->next; \
		} \
	} while (0)

void usbDriverClassInit(void *obj, const USBDeviceDescriptor *deviceDescr, const USBConfigDescriptor **configs,
		const USBStringDescriptor **strings, int stringCount) {
	USBDriverClass *usb = obj;
	usb->deviceDescr = deviceDescr;
	usb->configs = configs;
	usb->strings = strings;
	usb->stringCount = stringCount;
	usb->state = USB_DRIVER_RESET;
	usb->ep0RxBuffer = malloc(USB_MAX_EP0_PACKET_SIZE);
	usb->hooks = NULL;
}

void usbDriverEpWriteZLP(USBDriverClass *usb, uint8_t ep) {
	usbDriverEpWrite(usb, ep, NULL, 0);
	usbDriverEpStartTx(usb, ep);
}

void usbDriverEp0WriteZLP(USBDriverClass *usb) {
	usbDriverEpWriteZLP(usb, 0 | USB_ENDPOINT_IN);
}

static const USBDeviceDescriptor *usbDriverGetDeviceDescriptor(USBDriverClass *usb) {
	return usb->deviceDescr;
}

static const USBConfigDescriptor *usbDriverGetConfigDescriptor(USBDriverClass *usb, uint8_t index) {
	if (usb->configs && (index < usb->deviceDescr->bNumConfigurations)) {
		return usb->configs[index];
	}
	return NULL;
}

static const USBStringDescriptor *usbDriverGetStringDescriptor(USBDriverClass *usb, uint8_t index) {
	if (usb->strings && (index < usb->stringCount)) {
		return usb->strings[index];
	}
	return NULL;
}

static uint8_t usbDriverSetCurrentConfig(USBDriverClass *usb, uint8_t index) {
	int i;
	for (i = 0; i < usb->deviceDescr->bNumConfigurations; i++) {
		if (usb->configs[i]->bConfigurationValue == index) {
			usb->curConfig = usb->configs[i];
			usbDriverCallHook(usb, setConfigHook, usb, usb->curConfig);
			return 1;
		}
	}
	return 0;
}

static const USBDescriptor *usbDriverGetDescriptor(USBDriverClass *usb, uint8_t type, uint8_t index) {
	const USBDescriptor *descr = NULL;
	switch (type) {
		case USB_DESCR_DEVICE:
			descr = (const USBDescriptor*)usbDriverGetDeviceDescriptor(usb);
			break;
		case USB_DESCR_CONFIG:
			descr = (const USBDescriptor*)usbDriverGetConfigDescriptor(usb, index);
			break;
		case USB_DESCR_STRING:
			descr = (const USBDescriptor*)usbDriverGetStringDescriptor(usb, index);
			break;
	}
	return descr;
}

static uint8_t usbDriverHandleDeviceSetupRequest(USBDriverClass *usb, USBSetupRequest *rq) {
	switch (rq->bRequest) {
		case USB_REQ_STD_SET_ADDRESS:
			usb->state = USB_DRIVER_SET_ADDRESS;
			usb->address = rq->wValue & 0x7F;
			usbDriverEp0WriteZLP(usb);
			if (usb->earlySetAddress) {
				usbDriverSetAddress(usb, usb->address);
				usbDriverCallHook(usb, setAddressHook, usb, usb->address);
			}
			return 1;
		case USB_REQ_STD_GET_DESCRIPTOR: {
			uint8_t type = rq->wValue >> 8;
			uint8_t index = rq->wValue & 0xFF;
			const USBDescriptor *descr = usbDriverGetDescriptor(usb, type, index);
			if (descr) {
				uint16_t len = descr->bLength; 
				if (type == USB_DESCR_CONFIG) {
					len = ((const USBConfigDescriptor*)descr)->wTotalLength;
				}
				if (len > rq->wLength) {
					len = rq->wLength;
				}
				usbDriverEp0Write(usb, descr, len);
				return 1;
			}
			break;
		}
		case USB_REQ_STD_GET_STATUS:
			usbDriverEp0Write(usb, &usb->status, sizeof(uint16_t));
			return 1;
		case USB_REQ_STD_SET_FEATURE:
			if (rq->wValue == USB_DEVICE_FEATURE_REMOTE_WAKEUP) {
				usb->status |= USB_DEVICE_STATUS_REMOTE_WAKEUP;
			}
			usbDriverEp0WriteZLP(usb);
			return 1;
		case USB_REQ_STD_CLEAR_FEATURE:
			if (rq->wValue == USB_DEVICE_FEATURE_REMOTE_WAKEUP) {
				usb->status &= ~USB_DEVICE_STATUS_REMOTE_WAKEUP;
			}
			usbDriverEp0WriteZLP(usb);
			return 1;
		case USB_REQ_STD_SET_CONFIGURATION:
			if (usbDriverSetCurrentConfig(usb, rq->wValue)) {
				usbDriverEp0WriteZLP(usb);
				return 1;
			}
			break;
		case USB_REQ_STD_GET_CONFIGURATION: {
			uint8_t config = 0;
			if (usb->curConfig != NULL) {
				config = usb->curConfig->bConfigurationValue;
			}
			usbDriverEp0Write(usb, &config, sizeof(config));
			return 1;
		}
	}
	return 0;
}

static uint8_t usbDriverHandleInterfaceSetupRequest(USBDriverClass *usb, USBSetupRequest *rq) {
	return 0;
}

static uint8_t usbDriverHandleEndpointSetupRequest(USBDriverClass *usb, USBSetupRequest *rq) {
	return 0;
}

static uint8_t usbDriverHandleSetupRequest(USBDriverClass *usb, USBSetupRequest *rq, uint8_t *data, uint16_t dataLen) {
	USBDriverHooks *hooks = usb->hooks;
	while (hooks) {
		if (hooks->setupRequestHook) {
			if (hooks->setupRequestHook(usb, rq, data, dataLen, hooks->arg)) {
				return 1;
			}
		}
		hooks = hooks->next;
	}
	if ((rq->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD) {
		switch (rq->bmRequestType & USB_REQ_RECIPIENT_MASK) {
			case USB_REQ_RECIPIENT_DEVICE:
				return usbDriverHandleDeviceSetupRequest(usb, rq);
			case USB_REQ_RECIPIENT_INTERFACE:
				return usbDriverHandleInterfaceSetupRequest(usb, rq);
			case USB_REQ_RECIPIENT_ENDPOINT:
				return usbDriverHandleEndpointSetupRequest(usb, rq);
		}
	}
	return 0;
}

static void usbDriverControlEpOutCallback(USBDriverClass *usb, uint8_t ep, uint8_t setup, void *arg) {
	if (setup) {
		int len = usbDriverEpRead(usb, ep, &usb->setupRequest, sizeof(usb->setupRequest));
		usbDriverEpStartRx(usb, 0 | USB_ENDPOINT_OUT);
		if (len >= (int)sizeof(usb->setupRequest)) {
			if ((usb->setupRequest.bmRequestType & USB_REQ_DIR_MASK) == USB_REQ_DIR_HOST_TO_DEVICE) {
				usb->ep0RxCount = usb->setupRequest.wLength;
				usb->ep0RxOffset = 0;
			} else {
				usb->ep0RxCount = 0;
			}
		}
	} else {
		if (usb->ep0RxBuffer && usb->ep0RxCount) {
			int len = usbDriverEpRead(usb, ep, usb->ep0RxBuffer + usb->ep0RxOffset, usb->ep0RxCount);
			usb->ep0RxOffset += len;
			usb->ep0RxCount -= len;
		}
		usbDriverEpStartRx(usb, 0 | USB_ENDPOINT_OUT);
	}
	if (usb->ep0RxCount == 0) {
		if (!usbDriverHandleSetupRequest(usb, &usb->setupRequest, usb->ep0RxBuffer, usb->setupRequest.wLength)) {
			usbDriverEpStall(usb, 0 | USB_ENDPOINT_IN);
		}
	}
}

static void usbDriverEp0WriteChunk(USBDriverClass *usb) {
	if (usb->ep0TxBuffer) {
		if (usb->ep0TxCount < usb->deviceDescr->bMaxPacketSize) { // Last packet
			usbDriverEpNAK(usb, 0 | USB_ENDPOINT_OUT);
		}
		int count = usbDriverEpWrite(usb, 0 | USB_ENDPOINT_IN, usb->ep0TxBuffer, usb->ep0TxCount);
		usbDriverEpStartTx(usb, 0 | USB_ENDPOINT_IN);
		if (count > 0) {
			usb->ep0TxBuffer += count;
			usb->ep0TxCount -= count;
			if ((usb->ep0TxCount == 0) && (count < usb->deviceDescr->bMaxPacketSize)) {
				usb->ep0TxBuffer = 0;
			}
			return;
		}
	}
	usbDriverEpNAK(usb, 0 | USB_ENDPOINT_IN);
	usb->ep0TxCount = 0;
	usb->ep0TxBuffer = 0;
	usbDriverEpStartRx(usb, 0 | USB_ENDPOINT_OUT);
}

void usbDriverEp0Write(USBDriverClass *usb, const void *data, int count) {
	if (count > USB_MAX_EP0_PACKET_SIZE) count = USB_MAX_EP0_PACKET_SIZE;
	usb->ep0TxBuffer = (const uint8_t*)data;
	usb->ep0TxCount = count;
	usbDriverEp0WriteChunk(usb);
}

static void usbDriverControlEpInCallback(USBDriverClass *usb, uint8_t ep, uint8_t setup, void *arg) {
	if (usb->state == USB_DRIVER_SET_ADDRESS) {
		usb->state = USB_DRIVER_READY;
		if (!usb->earlySetAddress) {
			usbDriverEpNAK(usb, ep);
			usbDriverSetAddress(usb, usb->address);
			usbDriverCallHook(usb, setAddressHook, usb, usb->address);
		}
	} else {
		usbDriverEp0WriteChunk(usb);
	}
}

static void usbDriverControlEpCallback(USBDriverClass *usb, uint8_t ep, uint8_t setup, void *arg) {
	if (ep == 0) {
		usbDriverControlEpOutCallback(usb, ep, setup, arg);
	} else {
		usbDriverControlEpInCallback(usb, ep, setup, arg);
	}
}

void usbDriverHandleReset(void *obj) {
	USBDriverClass *usb = obj;
	usb->state = USB_DRIVER_RESET;
	usb->curConfig = 0;
	usbDriverSetAddress(usb, 0);
	usbDriverEpSetup(usb, 0, USB_ENDPOINT_CONTROL, usb->deviceDescr->bMaxPacketSize, usbDriverControlEpCallback, 0);
	usbDriverEpStartRx(usb, 0 | USB_ENDPOINT_OUT);
	usbDriverCallHook(usb, resetHook, usb);
}

void usbDriverHandleSuspend(void *obj) {
	USBDriverClass *usb = obj;
	usb->state = USB_DRIVER_SUSPEND;
	usbDriverCallHook(usb, suspendHook, usb);
}

void usbDriverHandleResume(void *obj) {
	USBDriverClass *usb = obj;
	usb->state = USB_DRIVER_READY;
	usbDriverCallHook(usb, resumeHook, usb);
}

void usbDriverRegisterHooks(void *obj, USBDriverHooks *hooks) {
	USBDriverClass *usb = obj;
	hooks->next = usb->hooks;
	usb->hooks = hooks;
}