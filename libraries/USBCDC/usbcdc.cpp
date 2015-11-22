#include <stdlib.h>
#include <string.h>
#include <usbcdc.h>

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

USBCDC::USBCDC(USBDriver& usb, uint8_t config, uint8_t cdcIface, uint8_t dataIface, int txBufferSize, int rxBufferSize) :
	USBDriverHook(usb), _txBuffer(txBufferSize), _rxBuffer(rxBufferSize)
{
	_config = config;
	_cdcIface = cdcIface;
	_dataIface = dataIface;
	_ready = false;
}

USBEndpointCallback USBCDC::endpointCallback(const USBConfigDescr *config, const USBInterfaceDescr *iface, const USBEndpointDescr *endpoint, void **arg) {
	if ((config->bConfigurationValue == _config) && (iface->bInterfaceNumber == _cdcIface) && ((endpoint->bmAttributes == USB_ENDPOINT_INTERRUPT))) {
		if ((endpoint->bEndpointAddress & 0x80) == USB_ENDPOINT_IN) {
			*arg = this;
			_intrEp = endpoint->bEndpointAddress;
			return interruptEpCallback;
		}
	} else if ((config->bConfigurationValue == _config) && (iface->bInterfaceNumber == _dataIface) && ((endpoint->bmAttributes == USB_ENDPOINT_BULK))) {
		*arg = this;
		if ((endpoint->bEndpointAddress & 0x80) == USB_ENDPOINT_IN) {
			_bulkInEp = endpoint->bEndpointAddress;
			_bulkInChunkSize = endpoint->wMaxPacketSize;
			return dataInEpCallback;
		} else {
			_bulkOutEp = endpoint->bEndpointAddress;
			_bulkOutChunkSize = endpoint->wMaxPacketSize;
			return dataOutEpCallback;
		}
	}
	return USBDriverHook::endpointCallback(config, iface, endpoint, arg);
}

void USBCDC::interruptEpCallback(USBDriver& usb, int ep, bool setup, void *arg) {
	USBCDC *usbCdc = (USBCDC*)arg;
	usbCdc->_usb.epNAK(ep);
}

void USBCDC::dataInEpCallback(USBDriver& usb, int ep, bool setup, void *arg) {
	USBCDC *usbCdc = (USBCDC*)arg;
	usbCdc->sendChunk();
}

void USBCDC::dataOutEpCallback(USBDriver& usb, int ep, bool setup, void *arg) {
	USBCDC *usbCdc = (USBCDC*)arg;
	usbCdc->recvChunk();
}

void USBCDC::handleReset() {
	_ready = false;
	USBDriverHook::handleReset();
}

void USBCDC::handleConfigChange(const USBConfigDescr *config) {
	_ready = (config != NULL);
	_usb.epStartRx(_bulkOutEp);
	sendChunk();
	USBDriverHook::handleConfigChange(config);
}

bool USBCDC::handleSetupRequest(USBDeviceRequest& rq, uint8_t *data, uint16_t dataLen) {
	if ((rq.bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) {
		switch (rq.bRequest) {
			case SET_LINE_CODING:
				if (data && (dataLen >= sizeof(_lineCoding))) {
					memmove(_lineCoding, data, sizeof(_lineCoding));
				}
				_usb.ep0WriteNull();
				return true;
			case GET_LINE_CODING:
				_usb.ep0Write(_lineCoding, sizeof(_lineCoding));
				return true;
			case SET_CONTROL_LINE_STATE:
				_usb.ep0WriteNull();
				return true;
		}
	}
	return USBDriverHook::handleSetupRequest(rq, data, dataLen);
}

void USBCDC::sendChunk() {
	if (_ready) {
		if (_tx) {
			char chunk[_bulkInChunkSize];
			int count = _txBuffer.read(chunk, sizeof(chunk), 0);
			_usb.epWrite(_bulkInEp, chunk, count);
			_usb.epStartTx(_bulkInEp);
			_tx = count == (int)sizeof(chunk);
		} else {
			_usb.epNAK(_bulkInEp);
		}
	}
}

void USBCDC::recvChunk() {
	char chunk[_bulkOutChunkSize];
	int len = _usb.epRead(_bulkOutEp, chunk, sizeof(chunk));
	_usb.epStartRx(_bulkOutEp);
	_rxBuffer.write(chunk, len, 0);
}

int USBCDC::writeData(const char *data, int len, int timeout) {
	int r = _txBuffer.write(data, len, timeout);
	if (!_tx) {
		_tx = true;
		sendChunk();
	}
	return r;
}

int USBCDC::readData(char *buffer, int len, int timeout) {
	return _rxBuffer.read(buffer, len, timeout);
}

void USBCDC::flush() {
	_rxBuffer.clear();
}