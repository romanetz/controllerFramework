#ifndef __USB_H__
#define __USB_H__

#include <usb-standard.h>

class USBDriver;

class USBDriverHook;

enum USBEndpointType {
	USB_ENDPOINT_CONTROL = 0,
	USB_ENDPOINT_ISOCHRONOUS = 1,
	USB_ENDPOINT_BULK = 2,
	USB_ENDPOINT_INTERRUPT = 3	
};

typedef void (*USBEndpointCallback)(USBDriver& usb, int ep, bool setup, void *arg);

enum USBDriverState {
	USB_DRIVER_RESET,
	USB_DRIVER_SUSPEND,
	USB_DRIVER_SET_ADDRESS,
	USB_DRIVER_READY
};

class USBDriver {
	private:
		const uint8_t *_ep0TxBuffer;
		
		uint16_t _ep0TxCount;
		
		uint8_t *_ep0RxBuffer;
		
		uint16_t _ep0RxOffset;
		
		uint16_t _ep0RxCount;
		
		USBDriverHook *_hook;
		
		USBDeviceRequest _setupRequest;
		
		static void usbDriverControlEpCallback(USBDriver& usb, int ep, bool setup, void *arg);
		
		void handleEP0Event(int ep, bool setup);
		
		bool handleSetupRequest(USBDeviceRequest& rq, uint8_t *data, uint16_t dataLen);
		
		bool handleDeviceSetupRequest(USBDeviceRequest& rq);
		
		bool handleInterfaceSetupRequest(USBDeviceRequest& rq);
		
		bool handleEndpointSetupRequest(USBDeviceRequest& rq);
		
		bool setCurrentConfig(uint8_t value);
		
	protected:
		USBDriverState _state;
		
		uint8_t _address;
		
		const USBDeviceDescr *_deviceDescr;
		
		const USBConfigDescr **_configs;
		
		const USBStringDescr **_strings;
		
		int _stringCount;
		
		const USBConfigDescr *_currentConfig;
		
		uint16_t _status;
		
		virtual void setAddress(uint8_t addr, bool early) = 0;
		
		virtual void handleReset();
		
		virtual void handleSuspend();
		
		virtual void handleResume();
		
		void ep0WriteNext();
		
	public:
		USBDriver(const USBDeviceDescr *deviceDescr, const USBConfigDescr **configs, const USBStringDescr **stringDescr, int stringCount);
		
		virtual void connect() = 0;
		
		virtual void disconnect() = 0;
		
		uint8_t address() { return _address; };
		
		USBDriverState state() { return _state; };
		
		virtual void resetAllEp() = 0;
		
		virtual bool epSetup(uint8_t ep, USBEndpointType type, uint16_t bufSize, USBEndpointCallback callback, void *arg) = 0;
		
		virtual void epStall(int ep) = 0;
		
		virtual int epWrite(int ep, const void *data, int dataLen) = 0;
		
		virtual void epStartTx(int ep) = 0;
		
		int epWriteNull(int ep);
		
		virtual void epNAK(int ep) = 0;
		
		virtual int epRead(int ep, void *buffer, int bufferSize) = 0;
		
		virtual void epStartRx(int ep) = 0;
		
		void ep0WriteNull();
		
		void ep0Write(const void *data, int dataLen);
		
		const USBDescriptor *getDescriptor(uint8_t type, uint8_t index);
		
		const USBDeviceDescr *deviceDescriptor();
		
		const USBConfigDescr *configDescriptor(uint8_t index);
		
		const USBStringDescr *stringDescriptor(uint8_t index);
		
		const USBConfigDescr *currentConfig() { return _currentConfig; }
		
		uint16_t status() { return _address; };
		
		void setBusPowered();
		
		void setSelfPowered();
		
		static const USBDescriptor* nextDescriptor(const USBDescriptor *descr, const void *end, uint8_t type = 0);
		
		void registerHook(USBDriverHook& hook);
		
};

class USBDriverHook {
	protected:
		USBDriver& _usb;
		
		USBDriverHook *_next;
		
	public:
		USBDriverHook(USBDriver& usb);
		
		void registered(USBDriverHook *next);
		
		virtual USBEndpointCallback endpointCallback(const USBConfigDescr *config, const USBInterfaceDescr *iface,
			const USBEndpointDescr *endpoint, void **arg);
		
		virtual void handleReset();
		
		virtual void handleSuspend();
		
		virtual void handleResume();
		
		virtual bool handleSetupRequest(USBDeviceRequest &rq, uint8_t *data, uint16_t dataLen);
		
		virtual void handleConfigChange(const USBConfigDescr *config);
		
};

#endif
