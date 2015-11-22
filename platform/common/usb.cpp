#include <stdlib.h>
#include <platform.h>
#include <usb.h>

USBDriver::USBDriver(const USBDeviceDescr *deviceDescr, const USBConfigDescr **configs, const USBStringDescr **strings, int stringCount)
	: _deviceDescr(deviceDescr), _configs(configs), _strings(strings), _stringCount(stringCount)
{
	_state = USB_DRIVER_SUSPEND;
}

void USBDriver::handleReset() {
	_state = USB_DRIVER_RESET;
	_address = 0x00;
	_status = 0x0000;
	_currentConfig = NULL;
	_ep0TxBuffer = NULL;
	if (_ep0RxBuffer) free(_ep0RxBuffer);
	_ep0RxBuffer = NULL;
	epSetup(0, USB_ENDPOINT_CONTROL, _deviceDescr->bMaxPacketSize, usbDriverControlEpCallback, 0);
	epStartRx(0 | USB_ENDPOINT_OUT);
	if (_hook) {
		_hook->handleReset();
	}
}

void USBDriver::handleSuspend() {
	_state = USB_DRIVER_SUSPEND;
	if (_hook) {
		_hook->handleSuspend();
	}
}

void USBDriver::handleResume() {
	_state = USB_DRIVER_READY;
	if (_hook) {
		_hook->handleResume();
	}
}

void USBDriver::usbDriverControlEpCallback(USBDriver& usb, int ep, bool setup, void *arg) {
	usb.handleEP0Event(ep, setup);
}

void USBDriver::handleEP0Event(int ep, bool setup) {
	if (ep == (0 | USB_ENDPOINT_OUT)) {
		if (setup) {
			int len = epRead(ep, &_setupRequest, sizeof(_setupRequest));
			epStartRx(0 | USB_ENDPOINT_OUT);
			if (len >= (int)sizeof(_setupRequest)) {
				if ((_setupRequest.bmRequestType & USB_REQ_DIR_MASK) == USB_REQ_DIR_HOST_TO_DEVICE) {
					if (_setupRequest.wLength > 0) {
						if (_ep0RxBuffer) free(_ep0RxBuffer);
						_ep0RxCount = _setupRequest.wLength;
						_ep0RxBuffer = (uint8_t*)malloc(_ep0RxCount);
						_ep0RxOffset = 0;
						return;
					}
				}
				if (handleSetupRequest(_setupRequest, NULL, 0)) {
					if (_ep0RxBuffer) free(_ep0RxBuffer);
					_ep0RxBuffer = NULL;
					return;
				} else {
					epStall(0 | USB_ENDPOINT_IN);
				}
			}
		} else {
			if (_ep0RxBuffer) {
				int len = epRead(ep, _ep0RxBuffer + _ep0RxOffset, _ep0RxCount);
				_ep0RxOffset += len;
				_ep0RxCount -= len;
			}
			epStartRx(0 | USB_ENDPOINT_OUT);
			if (_ep0RxBuffer && (_ep0RxCount == 0)) {
				if (handleSetupRequest(_setupRequest, _ep0RxBuffer, _setupRequest.wLength)) {
					free(_ep0RxBuffer);
					_ep0RxBuffer = NULL;
					return;
				} else {
					epStall(0 | USB_ENDPOINT_IN);
				}
			}
		}
	} else if (ep == (0 | USB_ENDPOINT_IN)) {
		if (_state == USB_DRIVER_SET_ADDRESS) {
			epNAK(ep);
			setAddress(_address, false);
			_state = USB_DRIVER_READY;
		} else {
			ep0WriteNext();
		}
	}
}

int USBDriver::epWriteNull(int ep) {
	int r = epWrite(ep, NULL, 0);
	if (r >= 0) {
		epStartTx(ep);
	}
	return r;
}

void USBDriver::ep0WriteNull() {
	epWriteNull(0 | USB_ENDPOINT_IN);
}

void USBDriver::ep0Write(const void *data, int dataLen) {
	_ep0TxBuffer = (const uint8_t*)data;
	_ep0TxCount = dataLen;
	ep0WriteNext();
}

void USBDriver::ep0WriteNext() {
	if (_ep0TxBuffer) {
		if (_ep0TxCount < _deviceDescr->bMaxPacketSize) { // Last packet
			epNAK(0 | USB_ENDPOINT_OUT);
		}
		int count = epWrite(0 | USB_ENDPOINT_IN, _ep0TxBuffer, _ep0TxCount);
		epStartTx(0 | USB_ENDPOINT_IN);
		if (count > 0) {
			_ep0TxBuffer += count;
			_ep0TxCount -= count;
			if ((_ep0TxCount == 0) && (count < _deviceDescr->bMaxPacketSize)) {
				_ep0TxBuffer = 0;
			}
			return;
		}
	}
	epNAK(0 | USB_ENDPOINT_IN);
	_ep0TxCount = 0;
	_ep0TxBuffer = 0;
	epStartRx(0 | USB_ENDPOINT_OUT);
}

bool USBDriver::handleSetupRequest(USBDeviceRequest& rq, uint8_t *data, uint16_t dataLen) {
	if (_hook) {
		if (_hook->handleSetupRequest(rq, data, dataLen)) {
			return true;
		}
	}
	if ((rq.bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD) {
		switch (rq.bmRequestType & USB_REQ_RECIPIENT_MASK) {
			case USB_REQ_RECIPIENT_DEVICE:
				return handleDeviceSetupRequest(rq);
			case USB_REQ_RECIPIENT_INTERFACE:
				return handleInterfaceSetupRequest(rq);
			case USB_REQ_RECIPIENT_ENDPOINT:
				return handleEndpointSetupRequest(rq);
		}
	}
	return false;
}

bool USBDriver::handleDeviceSetupRequest(USBDeviceRequest& rq) {
	switch (rq.bRequest) {
		case USB_REQ_STD_SET_ADDRESS:
			_state = USB_DRIVER_SET_ADDRESS;
			_address = rq.wValue & 0x7F;
			setAddress(_address, true);
			ep0WriteNull();
			return true;
		case USB_REQ_STD_GET_DESCRIPTOR: {
			uint8_t type = rq.wValue >> 8;
			uint8_t index = rq.wValue & 0xFF;
			const USBDescriptor *descr = getDescriptor(type, index);
			if (descr) {
				uint16_t len = descr->bLength; 
				if (type == USB_DESCR_CONFIG) {
					len = ((const USBConfigDescr*)descr)->wTotalLength;
				}
				if (len > rq.wLength) {
					len = rq.wLength;
				}
				ep0Write(descr, len);
				return true;
			}
			break;
		}
		case USB_REQ_STD_GET_STATUS:
			ep0Write(&_status, sizeof(uint16_t));
			return true;
		case USB_REQ_STD_SET_FEATURE:
			if (rq.wValue == USB_DEVICE_FEATURE_REMOTE_WAKEUP) {
				_status |= USB_DEVICE_STATUS_REMOTE_WAKEUP;
			}
			ep0WriteNull();
			return true;
		case USB_REQ_STD_CLEAR_FEATURE:
			if (rq.wValue == USB_DEVICE_FEATURE_REMOTE_WAKEUP) {
				_status &= ~USB_DEVICE_STATUS_REMOTE_WAKEUP;
			}
			ep0WriteNull();
			return true;
		case USB_REQ_STD_SET_CONFIGURATION:
			if (setCurrentConfig(rq.wValue)) {
				ep0WriteNull();
				return true;
			}
			break;
		case USB_REQ_STD_GET_CONFIGURATION: {
			uint8_t config = 0;
			if (_currentConfig != NULL) {
				config = _currentConfig->bConfigurationValue;
			}
			ep0Write(&config, sizeof(config));
			return true;
		}
	}
	return false;
}

bool USBDriver::handleInterfaceSetupRequest(USBDeviceRequest& rq) {
	return false;
}

bool USBDriver::handleEndpointSetupRequest(USBDeviceRequest& rq) {
	return false;
}

const USBDescriptor *USBDriver::getDescriptor(uint8_t type, uint8_t index) {
	const USBDescriptor *descr = NULL;
	switch (type) {
		case USB_DESCR_DEVICE:
			descr = (const USBDescriptor*)deviceDescriptor();
			break;
		case USB_DESCR_CONFIG:
			descr = (const USBDescriptor*)configDescriptor(index);
			break;
		case USB_DESCR_STRING:
			descr = (const USBDescriptor*)stringDescriptor(index);
			break;
	}
	return descr;
}

const USBDeviceDescr *USBDriver::deviceDescriptor() {
	return _deviceDescr;
}

const USBConfigDescr *USBDriver::configDescriptor(uint8_t index) {
	if (_configs && (index < _deviceDescr->bNumConfigurations)) {
		return _configs[index];
	}
	return NULL;
}

const USBStringDescr *USBDriver::stringDescriptor(uint8_t index) {
	if (_strings && (index < _stringCount)) {
		return _strings[index];
	}
	return NULL;
}

bool USBDriver::setCurrentConfig(uint8_t index) {
	for (int i = 0; i < _deviceDescr->bNumConfigurations; i++) {
		if (_configs[i]->bConfigurationValue == index) {
			_currentConfig = _configs[i];
			const USBDescriptor* descr = (const USBDescriptor*)_currentConfig;
			const char *end = (const char*)descr + _currentConfig->wTotalLength;
			const USBInterfaceDescr *iface = NULL;
			while ((descr = nextDescriptor(descr, end)) != NULL) {
				if (descr->bDescriptorType == USB_DESCR_INTERFACE) {
					iface = (const USBInterfaceDescr*)descr;
				} else if (descr->bDescriptorType == USB_DESCR_ENDPOINT) {
					const USBEndpointDescr *epDescr = (const USBEndpointDescr*)descr;
					USBEndpointCallback callback = NULL;
					void *arg = NULL;
					if (_hook) {
						callback = _hook->endpointCallback(_currentConfig, iface, epDescr, &arg);
					}
					epSetup(epDescr->bEndpointAddress, (USBEndpointType)(epDescr->bmAttributes & 3), epDescr->wMaxPacketSize,
						callback, arg);
				}
			}
			if (_hook) {
				_hook->handleConfigChange(_currentConfig);
			}
			return true;
		}
	}
	return false;
}

void USBDriver::setBusPowered() {
	_status &= USB_DEVICE_STATUS_SELF_POWERED;
}

void USBDriver::setSelfPowered() {
	_status |= USB_DEVICE_STATUS_SELF_POWERED;
}

const USBDescriptor* USBDriver::nextDescriptor(const USBDescriptor *descr, const void *end, uint8_t type) {
	if (descr == NULL) return NULL;
	const char *ptr = (const char*)descr;
	while (ptr < end) {
		const USBDescriptor *descr = (const USBDescriptor*)ptr;
		ptr += descr->bLength;
		if (ptr >= end) break;
		descr = (const USBDescriptor*)ptr;
		if ((type == 0) || (descr->bDescriptorType == type)) {
			return descr;
		}
	}
	return NULL;
}

void USBDriver::registerHook(USBDriverHook& hook) {
	USBDriverHook *prev = _hook;
	_hook = &hook;
	hook.registered(prev);
}

USBDriverHook::USBDriverHook(USBDriver& usb) : _usb(usb) {
	_usb.registerHook(*this);
}

void USBDriverHook::registered(USBDriverHook* next) {
	_next = next;
}

USBEndpointCallback USBDriverHook::endpointCallback(const USBConfigDescr *config, const USBInterfaceDescr *iface,
		const USBEndpointDescr *endpoint, void **arg) {
	if (_next) {
		return _next->endpointCallback(config, iface, endpoint, arg);
	}
	return NULL;
}

void USBDriverHook::handleReset() {
	if (_next) {
		_next->handleReset();
	}
}

void USBDriverHook::handleSuspend() {
	if (_next) {
		_next->handleSuspend();
	}
}

void USBDriverHook::handleResume() {
	if (_next) {
		_next->handleResume();
	}
}

bool USBDriverHook::handleSetupRequest(USBDeviceRequest &rq, uint8_t *data, uint16_t dataLen) {
	if (_next) {
		return _next->handleSetupRequest(rq, data, dataLen);
	}
	return false;
}

void USBDriverHook::handleConfigChange(const USBConfigDescr *config) {
	if (_next) {
		_next->handleConfigChange(config);
	}
}
