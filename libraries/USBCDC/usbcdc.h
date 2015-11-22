#ifndef __USB_CDC_H__
#define __USB_CDC_H__

#include <usb.h>
#include <iochannel.h>
#include <memory-buffer.h>

class USBCDC: public IOChannel, public USBDriverHook {
	private:
		bool _ready;
		
		uint8_t _config;
		
		uint8_t _cdcIface;
		
		uint8_t _dataIface;
		
		uint8_t _intrEp;
		
		uint8_t _bulkOutEp;
		
		uint16_t _bulkOutChunkSize;
		
		uint8_t _bulkInEp;
		
		uint16_t _bulkInChunkSize;
		
		uint8_t _lineCoding[7];
		
		MemoryBuffer _txBuffer;
		
		MemoryBuffer _rxBuffer;
		
		volatile bool _tx;
		
		static void interruptEpCallback(USBDriver& usb, int ep, bool setup, void *arg);
		
		static void dataInEpCallback(USBDriver& usb, int ep, bool setup, void *arg);
		
		static void dataOutEpCallback(USBDriver& usb, int ep, bool setup, void *arg);
		
		void sendChunk();
		
		void recvChunk();
		
	protected:
		int writeData(const char *data, int len, int timeout);
		
		int readData(char *buffer, int len, int timeout);
		
	public:
		USBCDC(USBDriver& usb, uint8_t config, uint8_t cdcIface, uint8_t dataIface, int txBufferSize = 128, int rxBufferSize = 128);
		
		USBEndpointCallback endpointCallback(const USBConfigDescr *config, const USBInterfaceDescr *iface,
			const USBEndpointDescr *endpoint, void **arg);
		
		void handleReset();
		
		void handleConfigChange(const USBConfigDescr *config);
		
		bool handleSetupRequest(USBDeviceRequest& rq, uint8_t *data, uint16_t dataLen);
		
		bool ready() { return _ready; };
		
};

#endif
