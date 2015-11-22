#ifndef __STM32_USB_H__
#define __STM32_USB_H__

#include <usb.h>

#ifdef STM32F1

struct STM32USBRegs {
	uint32_t EPR[8];
	uint32_t RESERVED[8];
	uint32_t CNTR;
	uint32_t ISTR;
	uint32_t FNR;
	uint32_t DADDR;
	uint32_t BTABLE;
};

struct STM32USBBufferDesc {
	uint32_t txAddr;
	uint32_t txCount;
	uint32_t rxAddr;
	uint32_t rxCount;
};

enum STM32USBEpType {
	STM32USB_EP_BULK = 0,
	STM32USB_EP_CONTROL = 1,
	STM32USB_EP_ISO = 2,
	STM32USB_EP_INTERRUPT = 3
};

enum STM32USBEpStatus {
	STM32USB_EP_DISABLED = 0,
	STM32USB_EP_STALL = 1,
	STM32USB_EP_NAK = 2,
	STM32USB_EP_VALID = 3
};

struct STM32USBEpInfo {
	uint8_t addr;
	uint16_t txBufSize;
	USBEndpointCallback inCallback;
	void *inCallbackArg;
	USBEndpointCallback outCallback;
	void *outCallbackArg;
};

#define STM32USB_MAX_EP_COUNT 8

#define USB_PMA_BASE (USB_BASE + 1024)

#define USB MMIO(USB_BASE, STM32USBRegs)

class STM32USBDriver: public USBDriver {
	private:
		uint16_t pmaFreeOffset;
		
		STM32USBEpInfo epInfo[STM32USB_MAX_EP_COUNT];
		
		void epSetTxStatus(uint8_t i, STM32USBEpStatus status);
		
		STM32USBEpStatus epTxStatus(uint8_t i) {
			return (STM32USBEpStatus)((USB->EPR[i] & USB_EP0R_STAT_TX_MASK) >> USB_EP0R_STAT_TX_OFFSET);
		}
		
		void epSetRxStatus(uint8_t i, STM32USBEpStatus status);
		
		STM32USBEpStatus epRxStatus(uint8_t i) {
			return (STM32USBEpStatus)((USB->EPR[i] & USB_EP0R_STAT_RX_MASK) >> USB_EP0R_STAT_RX_OFFSET);
		}
		
		volatile STM32USBBufferDesc *bufferTable() {
			return (volatile STM32USBBufferDesc*)(USB_PMA_BASE + USB->BTABLE * 2);
		}
		
		void copyToBufferArea(uint32_t offset, const void *data, int count);
		
		void copyFromBufferArea(void *ptr, uint32_t offset, int count);
		
	protected:
		void setAddress(uint8_t addr, bool early);
		
		void handleReset();
		
	public:
		STM32USBDriver(const USBDeviceDescr *deviceDescr, const USBConfigDescr **configs,
			const USBStringDescr **strings, int stringCount, bool connect = false);
		
		~STM32USBDriver();
		
		void connect();
		
		void disconnect();
		
		void resetAllEp();
		
		bool epSetup(uint8_t ep, USBEndpointType type, uint16_t bufSize, USBEndpointCallback callback, void *arg);
		
		void epStall(int ep);
		
		int epWrite(int ep, const void *data, int dataLen);
		
		void epStartTx(int ep);
		
		void epNAK(int ep);
		
		int epRead(int ep, void *buffer, int bufferSize);
		
		void epStartRx(int ep);
		
		void handleInterruptRequest();
		
};

#endif

#endif
