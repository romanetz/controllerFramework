#include <stdlib.h>
#include <usb-cdc.h>

#ifdef USE_CLASSES

enum {
	SEND_ENCAPSULATED_COMMAND = 0,
	GET_ENCAPSULATED_RESPONSE,
	SET_COMM_FEATURE,
	GET_COMM_FEATURE,
	CLEAR_COMM_FEATURE,
	SET_LINE_CODING = 0x20,
	GET_LINE_CODING,
	SET_CONTROL_LINE_STATE,
	SEND_BREAK
};

static void USBCDCClass_sendChunk(USBCDCClass *usbCdc) {
	if (usbDriverEpReady(usbCdc->usb, usbCdc->bulkInEp)) {
		uint8_t buffer[usbCdc->bulkInChunkSize];
		int count = queueReadTimeout(&usbCdc->txQueue, buffer, sizeof(buffer), 0);
		if ((count == 0) && (usbCdc->lastChunkSize == 0)) {
			usbDriverEpNAK(usbCdc->usb, usbCdc->bulkInEp);
			return;
		}
		usbDriverEpWrite(usbCdc->usb, usbCdc->bulkInEp, buffer, count);
		usbDriverEpStartTx(usbCdc->usb, usbCdc->bulkInEp);
		usbCdc->lastChunkSize = count;
	}
}

static void USBCDCClass_bulkInCallback(USBDriverClass *usb, uint8_t ep, uint8_t setup, void *arg) {
	USBCDCClass *usbCdc = USB_CDC_CLASS(arg);
	USBCDCClass_sendChunk(usbCdc);
}

static void USBCDCClass_bulkOutCallback(USBDriverClass *usb, uint8_t ep, uint8_t setup, void *arg) {
	USBCDCClass *usbCdc = USB_CDC_CLASS(arg);
	uint8_t buffer[usbCdc->bulkOutChunkSize];
	int count = usbDriverEpRead(usb, ep, buffer, sizeof(buffer));
	usbDriverEpStartRx(usb, ep);
	if (count > 0) {
		queueWriteTimeout(&usbCdc->rxQueue, buffer, count, 0);
	}
}

int USBCDCClass_writeTimeout(IOStreamClass *stream, const void *data, int len, timestamp_t timeout) {
	USBCDCClass *usbCdc = USB_CDC_CLASS(stream);
	int r = queueWriteTimeout(&usbCdc->txQueue, data, len, timeout);
	USBCDCClass_sendChunk(usbCdc);
	return r;
}

int USBCDCClass_readTimeout(IOStreamClass *stream, void *buffer, int len, timestamp_t timeout) {
	USBCDCClass *usbCdc = USB_CDC_CLASS(stream);
	return queueReadTimeout(&usbCdc->rxQueue, buffer, len, timeout);
}

void USBCDCClass_flush(IOStreamClass *stream) {
	USBCDCClass *usbCdc = USB_CDC_CLASS(stream);
	queueClear(&usbCdc->txQueue);
	queueClear(&usbCdc->rxQueue);
}

void USBCDCClass_close(IOStreamClass *stream) {
	//USBCDCClass *usbCdc = USB_CDC_CLASS(stream);
}

void USBCDCClass_resetHook(USBDriverClass *usb, void *arg) {
	//USBCDCClass *usbCdc = USB_CDC_CLASS(arg);
}

void USBCDCClass_setConfigHook(USBDriverClass *usb, const USBConfigDescriptor *config, void *arg) {
	USBCDCClass *usbCdc = USB_CDC_CLASS(arg);
	if (config) {
		usbDriverEpSetup(usb, usbCdc->bulkOutEp, USB_ENDPOINT_BULK, usbCdc->bulkOutChunkSize, USBCDCClass_bulkOutCallback, usbCdc);
		usbDriverEpSetup(usb, usbCdc->bulkInEp, USB_ENDPOINT_BULK, usbCdc->bulkInChunkSize, USBCDCClass_bulkInCallback, usbCdc);
		//usbDriverEpSetup(usb, usbCdc->intrInEp, USB_ENDPOINT_INTERRUPT, usbCdc->intrInChunkSize, USBCDCClass_intrInCallback, usbCdc);
		usbDriverEpStartRx(usb, usbCdc->bulkOutEp);
		USBCDCClass_sendChunk(usbCdc);
	}
}

BOOL USBCDCClass_setupRequestHook(USBDriverClass *usb, USBSetupRequest *rq, uint8_t *data, uint16_t dataLen, void *arg) {
	USBCDCClass *usbCdc = USB_CDC_CLASS(arg);
	if ((rq->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) {
		switch (rq->bRequest) {
			case SET_LINE_CODING:
				if (data && (dataLen >= sizeof(usbCdc->lineCoding))) {
					memmove(usbCdc->lineCoding, data, sizeof(usbCdc->lineCoding));
				}
				usbDriverEp0WriteZLP(usb);
				return 1;
			case GET_LINE_CODING:
				usbDriverEp0Write(usb, usbCdc->lineCoding, sizeof(usbCdc->lineCoding));
				return 1;
			case SET_CONTROL_LINE_STATE:
				usbDriverEp0WriteZLP(usb);
				return 1;
		}
	}
	return 0;
}

USBCDCClass *usbCdcClassInit(USBCDCClass *usbCdc, USBDriverClass *usb, int txBufferSize, int rxBufferSize,
		uint8_t bulkOutEp, uint16_t bulkOutChunkSize, uint8_t bulkInEp, uint16_t bulkInChunkSize, uint8_t intrInEp, uint16_t intrInChunkSize) {
	IO_STREAM_CLASS(usbCdc)->writeTimeout = USBCDCClass_writeTimeout;
	IO_STREAM_CLASS(usbCdc)->readTimeout = USBCDCClass_readTimeout;
	IO_STREAM_CLASS(usbCdc)->flush = USBCDCClass_flush;
	IO_STREAM_CLASS(usbCdc)->close = USBCDCClass_close;
	queueInit(&usbCdc->txQueue, 1, txBufferSize);
	queueInit(&usbCdc->rxQueue, 1, rxBufferSize);
	usbCdc->usb = usb;
	usbCdc->bulkOutChunkSize = bulkOutChunkSize;
	usbCdc->bulkInChunkSize = bulkInChunkSize;
	usbCdc->intrInChunkSize = intrInChunkSize;
	usbCdc->bulkOutEp = bulkOutEp;
	usbCdc->bulkInEp = bulkInEp;
	usbCdc->intrInEp = intrInEp;
	usbCdc->lastChunkSize = 0;
	USBDriverHooks *hooks = calloc(1, sizeof(USBDriverHooks));
	hooks->arg = usbCdc;
	hooks->resetHook = USBCDCClass_resetHook;
	hooks->setConfigHook = USBCDCClass_setConfigHook;
	hooks->setupRequestHook = USBCDCClass_setupRequestHook;
	usbDriverRegisterHooks(usb, hooks);
	return usbCdc;
}

#endif