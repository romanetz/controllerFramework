/**
	@addtogroup platform Platform-independent
	@{
	@addtogroup usb USB
	@{
*/
#ifndef __USB_H__
#define __USB_H__

#include <usb-standard.h>

class USBDriver;

class USBDriverHook;

/**
	@brief USB endpoint type.
*/
enum USBEndpointType {
	USB_ENDPOINT_CONTROL = 0, ///< Control endpoint.
	USB_ENDPOINT_ISOCHRONOUS = 1, ///< Isochronous endpoint.
	USB_ENDPOINT_BULK = 2, ///< Bulk endpoint.
	USB_ENDPOINT_INTERRUPT = 3 ///< Interrupt endpoint.
};

/**
	@brief USB endpoint callback function type.
	@param[in] usb Reference to USB driver.
	@param[in] ep Endpoint address.
	@param[in] setup Setup packet flag.
	@param[in] arg User data.
*/
typedef void (*USBEndpointCallback)(USBDriver& usb, int ep, bool setup, void *arg);

/**
	@brief USB driver state machine
*/
enum USBDriverState {
	USB_DRIVER_RESET, ///< USB peripheral after RESET event.
	USB_DRIVER_SUSPEND, ///< USB peripheral in the SUSPEND state.
	USB_DRIVER_SET_ADDRESS, ///< USB peripheral waiting for address assignment.
	USB_DRIVER_READY
};

/**
	@brief USB driver (device mode) base class.
*/
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
		/**
			@param[in] deviceDesr USB device descriptor.
			@param[in] configs Array of USB device configurations. It should contain deviceDescr->bNumConfigurations elements.
			@param[in] strings Array of USB string descriptors. It should contain stringCount elements. It can be null.
			@param[in] stringCount Count of USB strings (including string 0 - language code).
		*/
		USBDriver(const USBDeviceDescr *deviceDescr, const USBConfigDescr **configs, const USBStringDescr **strings, int stringCount);
		
		virtual ~USBDriver() {};
		
		/**
			@brief Enable host to detect USB device.
		*/
		virtual void connect() = 0;
		
		/**
			@brief Disconnect USB device from the bus.
		*/
		virtual void disconnect() = 0;
		
		/**
			@brief Get address which host assigned to the device.
			@return Device address (0 if device not enumerated yet).
		*/
		uint8_t address() { return _address; };
		
		/**
			@brief Get driver state machine.
			@return Driver state.
		*/
		USBDriverState state() { return _state; };
		
		/**
			@brief Reset all endpoints to inactive state.
		*/
		virtual void resetAllEp() = 0;
		
		/**
			@brief Setup device endpoint.
			@param[in] ep Endpoint address.
			@param[in] type Desired endpoint type.
			@param[in] bufSize TX/RX buffer size.
			@param[in] callback Function which will be called when data chunk received/transmitted.
			@param[in] arg User data for the callback function.
			@return Returns true if endpoint successfully set or false if not (out of memory, invalid endpoint address).
		*/
		virtual bool epSetup(uint8_t ep, USBEndpointType type, uint16_t bufSize, USBEndpointCallback callback, void *arg) = 0;
		
		/**
			@brief Set endpoint to stall state.
			@param[in] ep Endpoint address.
		*/
		virtual void epStall(int ep) = 0;
		
		/**
			@brief Write data to endpoint transmit buffer.
			This function only stores data in the buffer. For real transfer started you should call method epStartTx.
			@param[in] ep Endpoint address.
			@param[in] data Pointer to the data.
			@param[in] dataLen Length of data in bytes.
			@return Actual number of bytes that has been written (zero if endpoint busy, not greater than endpoint buffer size).
		*/
		virtual int epWrite(int ep, const void *data, int dataLen) = 0;
		
		/**
			@brief Start data transmitting from specified endpoint.
			@param[in] ep Endpoint address.
		*/
		virtual void epStartTx(int ep) = 0;
		
		/**
			@brief Send ZLP (Zero Length Packet) from specifiend endpoint.
			@param[in] ep Endpoint address.
		*/
		int epWriteNull(int ep);
		
		/**
			@brief Set specified endpoint to NAK state.
			@param[in] ep Endpoint address.
		*/
		virtual void epNAK(int ep) = 0;
		
		/**
			@brief Read data from endpoint receive buffer.
			@param[in] ep Endpoint address.
			@param[in] buffer Pointer to buffer for data.
			@param[in] bufferSize Size of specified buffer.
			@return Count of bytes copied from endpoint memory to buffer (zero if no data available).
		*/
		virtual int epRead(int ep, void *buffer, int bufferSize) = 0;
		
		/**
			@brief Enable data receiving on the specified endpoint.
			@param[in] ep Endpoint address.
		*/
		virtual void epStartRx(int ep) = 0;
		
		/**
			@brief Send ZLP (Zero Length Packet) from endpoint 0.
		*/
		void ep0WriteNull();
		
		/**
			@brief Send long data packet from endpoint 0 (data splitted into chunks).
			@param[in] data Pointer to data for transmit (must be valid until data fully transmitted).
			@param[in] dataLen Length of the datain bytes.
			If dataLen <= EP0 OUT buffer size, function sents data instantly, otherwise it send first data chunk and exits.
			Next chunks will be sent by interrupt handler.
		*/
		void ep0Write(const void *data, int dataLen);
		
		/**
			@brief Get USB descriptor by it index and type.
			@param[in] type Type of requested descriptor.
			@param[in] index Index of requested descriptor.
			@return Pointer to descriptor structure or NULL if descriptor not found.
		*/
		const USBDescriptor *getDescriptor(uint8_t type, uint8_t index);
		
		/**
			@brief Get USB device descriptor.
			@return Pointer to USB device descriptor structure.
		*/
		const USBDeviceDescr *deviceDescriptor();
		
		/**
			@brief Get USB configuration descriptor.
			@param[in] index Index of requested descriptor.
			@return Pointer to USB configuration descriptor or NULL.
		*/
		const USBConfigDescr *configDescriptor(uint8_t index);
		
		/**
			@brief Get USB string descriptor.
			@param[in] index Index of requested descriptor.
			@return Pointer to USB string descriptor or NULL.
		*/
		const USBStringDescr *stringDescriptor(uint8_t index);
		
		/**
			@brief Get current USB device configuration.
			@return Pointer to current configuration (NULL if device not configured)
		*/
		const USBConfigDescr *currentConfig() { return _currentConfig; }
		
		/**
			@brief Get USB device current status.
			@return Device status bitmask.
		*/
		uint16_t status() { return _address; };
		
		/**
			@brief Set device if bus powered.
		*/
		void setBusPowered();
		
		/**
			@brief Set device if self powered.
		*/
		void setSelfPowered();
		
		/**
			@brief Find next descriptor inlined to configuration descriptor.
			@param[in] descr Current search position.
			@param[in] end End of configuration descriptor.
			@param[in] type Desired descriptor type (zero - any descriptor).
			@return Pointer to next descriptor or NULL if end reached.
		*/
		static const USBDescriptor* nextDescriptor(const USBDescriptor *descr, const void *end, uint8_t type = 0);
		
		/**
			@brief Register USB driver hook.
			Hooks used for implement various device classes.
			@param[in] hook Hook object.
		*/
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
/**
	@}
	@}
*/