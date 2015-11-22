#include <stm32.h>

#ifdef STM32F1

static STM32USBDriver *usbDriver;

STM32USBDriver::STM32USBDriver(const USBDeviceDescr *deviceDescr, const USBConfigDescr **configs,
	const USBStringDescr **strings, int stringCount, bool connect) : USBDriver(deviceDescr, configs, strings, stringCount)
{
	if (connect) {
		this->connect();
	}
}

void STM32USBDriver::connect() {
	usbDriver = this;
	STM32RCC::periphClockEnable(USB);
	USB->CNTR = USB_CNTR_FRES;
	usleep(1);
	USB->CNTR &= ~USB_CNTR_FRES;
	USB->BTABLE = 0;
	USB->ISTR = 0;
	NVIC::enableIRQ(USB_FS_WKUP_IRQ);
	NVIC::enableIRQ(CAN1_RX0_IRQ);
	USB->CNTR = USB_CNTR_RESETM;
}

void STM32USBDriver::disconnect() {
	USB->CNTR = USB_CNTR_FRES;
	NVIC::disableIRQ(USB_FS_WKUP_IRQ);
	NVIC::disableIRQ(CAN1_RX0_IRQ);
	STM32RCC::periphClockDisable(USB);
	usbDriver = 0;
}

void STM32USBDriver::copyToBufferArea(uint32_t offset, const void *data, int count) {
	count = (count + 1) / 2;
	uint32_t *dst = (uint32_t*)(USB_PMA_BASE + offset * 2);
	const uint16_t *src = (const uint16_t*)data;
	while(count--) {
		*(dst++) = *(src++);
	}
}

void STM32USBDriver::copyFromBufferArea(void *ptr, uint32_t offset, int count) {
	count = (count + 1) / 2;
	uint16_t *dst = (uint16_t*)ptr;
	const uint32_t *src = (uint32_t*)(USB_PMA_BASE + offset * 2);
	while(count--) {
		*(dst++) = *(src++);
	}
}

void STM32USBDriver::setAddress(uint8_t addr, bool early) {
	if (!early) {
		USB->DADDR = addr | USB_DADDR_EF;
	}
}

void STM32USBDriver::epSetTxStatus(uint8_t i, STM32USBEpStatus status) {
	uint32_t epr = USB->EPR[i];
	epr &= ~(USB_EP0R_STAT_RX_MASK | USB_EP0R_DTOG_RX | USB_EP0R_STAT_TX_MASK | USB_EP0R_DTOG_TX | USB_EP0R_CTR_TX);
	epr |= USB_EP0R_CTR_RX;
	epr |= (USB->EPR[i] & USB_EP0R_STAT_TX_MASK) ^ (status << USB_EP0R_STAT_TX_OFFSET);
	USB->EPR[i] = epr;
}

void STM32USBDriver::epSetRxStatus(uint8_t i, STM32USBEpStatus status) {
	uint32_t epr = USB->EPR[i];
	epr &= ~(USB_EP0R_STAT_RX_MASK | USB_EP0R_DTOG_RX | USB_EP0R_STAT_TX_MASK | USB_EP0R_DTOG_TX | USB_EP0R_CTR_RX);
	epr |= USB_EP0R_CTR_TX;
	epr |= (USB->EPR[i] & USB_EP0R_STAT_RX_MASK) ^ (status << USB_EP0R_STAT_RX_OFFSET);
	USB->EPR[i] = epr;
}

void STM32USBDriver::resetAllEp() {
	for (int i = 0; i < 8; i++) {
		epInfo[i].inCallback = 0;
		epInfo[i].outCallback = 0;
		epInfo[i].txBufSize = 0;
		uint32_t epr = USB->EPR[i];
		epr &= ~USB_EP0R_EA_MASK;
		epr |= (epr & USB_EP0R_STAT_TX_MASK) ^ (STM32USB_EP_DISABLED << USB_EP0R_STAT_TX_OFFSET);
		epr |= (epr & USB_EP0R_STAT_RX_MASK) ^ (STM32USB_EP_DISABLED << USB_EP0R_STAT_RX_OFFSET);
		epr |= i;
		USB->EPR[i] = epr;
	}
	pmaFreeOffset = 8 * STM32USB_MAX_EP_COUNT;
}

bool STM32USBDriver::epSetup(uint8_t ep, USBEndpointType type, uint16_t bufSize, USBEndpointCallback callback, void *arg) {
	uint8_t dir = ep & USB_ENDPOINT_IN;
	ep &= 0x7F;
	if (ep >= STM32USB_MAX_EP_COUNT) return false;
	uint32_t epType;
	switch (type) {
		case USB_ENDPOINT_CONTROL:
			epType = 1;
			break;
		case USB_ENDPOINT_ISOCHRONOUS:
			epType = 2;
			break;
		case USB_ENDPOINT_INTERRUPT:
			epType = 3;
			break;
		case USB_ENDPOINT_BULK:
		default:
			epType = 0;
	}
	volatile STM32USBBufferDesc* bufferDesc = bufferTable() + ep;
	if (dir || (ep == 0)) {
		epInfo[ep].inCallback = callback;
		epInfo[ep].inCallbackArg = arg;
		bufSize = (bufSize + 1) & ~1;
		epInfo[ep].txBufSize = bufSize;
		bufferDesc->txAddr = pmaFreeOffset;
		pmaFreeOffset += bufSize;
	}
	if (!dir) {
		epInfo[ep].outCallback = callback;
		epInfo[ep].outCallbackArg = arg;
		bufSize = (bufSize + 1) & ~1;
		if (bufSize <= 62) {
			bufferDesc->rxCount = (bufSize / 2) << 10;
		} else {
			bufSize = (bufSize + 31) & ~31;
			bufferDesc->rxCount = 0x8000 | ((bufSize / 32) - 1) << 10;
		}
		bufferDesc->rxAddr = pmaFreeOffset;
		pmaFreeOffset += bufSize;
	}
	uint32_t origEpr = USB->EPR[ep];
	uint32_t epr = origEpr &
		~(USB_EP0R_STAT_RX_MASK | USB_EP0R_DTOG_RX | USB_EP0R_STAT_TX_MASK | USB_EP0R_DTOG_TX | USB_EP0R_EP_TYPE_MASK | USB_EP0R_EA_MASK);
	if (dir || (ep == 0)) {
		epr |= origEpr & USB_EP0R_DTOG_TX;
		epr |= USB_EP0R_CTR_RX;
		epr |= (origEpr & USB_EP0R_STAT_TX_MASK) ^ (STM32USB_EP_NAK << USB_EP0R_STAT_TX_OFFSET);
	}
	if (!dir) {
		epr |= origEpr & USB_EP0R_DTOG_RX;
		epr |= USB_EP0R_CTR_TX;
		epr |= (origEpr & USB_EP0R_STAT_RX_MASK) ^ (STM32USB_EP_NAK << USB_EP0R_STAT_RX_OFFSET);
	}
	epr |= epType << USB_EP0R_EP_TYPE_OFFSET;
	epr |= ep << USB_EP0R_EA_OFFSET;
	USB->EPR[ep] = (uint16_t)epr;
	epInfo[ep].addr = ep;
	return true;
}

void STM32USBDriver::epStall(int ep) {
	uint8_t dir = ep & USB_ENDPOINT_IN;
	ep &= 0x7F;
	if (dir) {
		epSetTxStatus(ep, STM32USB_EP_STALL);
	} else {
		epSetRxStatus(ep, STM32USB_EP_STALL);
	}
}

int STM32USBDriver::epWrite(int ep, const void *data, int dataLen) {
	if ((ep & USB_ENDPOINT_IN) == 0) return -1;
	ep &= 0x7F;
	if (epTxStatus(ep) == STM32USB_EP_VALID) return 0;
	if (dataLen > epInfo[ep].txBufSize) dataLen = epInfo[ep].txBufSize;
	volatile STM32USBBufferDesc *bufferDesc = bufferTable() + ep;
	copyToBufferArea(bufferDesc->txAddr, data, dataLen);
	bufferDesc->txCount = dataLen;
	return dataLen;
}

void STM32USBDriver::epStartTx(int ep) {
	ep &= 0x7F;
	epSetTxStatus(ep, STM32USB_EP_VALID);
}

void STM32USBDriver::epNAK(int ep) {
	uint8_t dir = ep & USB_ENDPOINT_IN;
	ep &= 0x7F;
	if (dir) {
		epSetTxStatus(ep, STM32USB_EP_NAK);
	} else {
		epSetRxStatus(ep, STM32USB_EP_NAK);
	}
}

int STM32USBDriver::epRead(int ep, void *buffer, int bufferSize) {
	if (ep & USB_ENDPOINT_IN) return -1;
	if (epRxStatus(ep) == STM32USB_EP_VALID) {
		return 0;
	}
	volatile STM32USBBufferDesc *bufferDesc = bufferTable() + ep;
	uint16_t len = bufferDesc->rxCount & 0x3FF;
	if (len > bufferSize) len = bufferSize;
	copyFromBufferArea(buffer, bufferDesc->rxAddr, len);
	return len;
}

void STM32USBDriver::epStartRx(int ep) {
	ep &= 0x7F;
	epSetRxStatus(ep, STM32USB_EP_VALID);
}

void STM32USBDriver::handleReset() {
	USB->BTABLE = 0;
	USB->ISTR = 0;
	pmaFreeOffset = 8 * STM32USB_MAX_EP_COUNT;
	USBDriver::handleReset();
	USB->DADDR = USB_DADDR_EF;
	USB->CNTR = USB_CNTR_RESETM | USB_CNTR_CTRM | USB_CNTR_SUSPM;
}

void STM32USBDriver::handleInterruptRequest() {
	uint16_t istr = USB->ISTR;
	if (istr & USB_ISTR_RESET) {
		handleReset();
		USB->ISTR = ~USB_ISTR_RESET;
		return;
	}
	if (istr & USB_ISTR_CTR) {
		uint8_t ep = istr & USB_ISTR_EP_ID_MASK;
		if (istr & USB_ISTR_DIR) {
			if (epInfo[ep].outCallback) {
				epInfo[ep].outCallback(*this, ep | USB_ENDPOINT_OUT, USB->EPR[ep] & USB_EP0R_SETUP, epInfo[ep].outCallbackArg);
			}
		} else {
			if (epInfo[ep].inCallback) {
				epInfo[ep].inCallback(*this, ep | USB_ENDPOINT_IN, false, epInfo[ep].inCallbackArg);
			}
		}
		uint32_t epr = USB->EPR[ep];
		epr &= ~(USB_EP0R_STAT_RX_MASK | USB_EP0R_DTOG_RX | USB_EP0R_STAT_TX_MASK | USB_EP0R_DTOG_TX | USB_EP0R_CTR_TX | USB_EP0R_CTR_RX);
		if (istr & USB_ISTR_DIR) {
			if (!epInfo[ep].outCallback) {
				epr |= USB_EP0R_CTR_TX; // Clear RX flag
				USB->EPR[ep] = epr;
			}
		} else {
			if (!epInfo[ep].inCallback) {
				epr |= USB_EP0R_CTR_RX; // Clear TX flag
				USB->EPR[ep] = epr;
			}
		}
	}
	if ((istr & USB_ISTR_SUSP) && (_state != USB_DRIVER_SUSPEND)) {
		USB->ISTR = ~USB_ISTR_SUSP;
		USB->CNTR = USB->CNTR = USB_CNTR_RESETM | USB_CNTR_CTRM | USB_CNTR_WKUPM | USB_CNTR_FSUSP;
		handleSuspend();
	}
	if ((istr & USB_ISTR_WKUP) && (_state == USB_DRIVER_SUSPEND)) {
		USB->ISTR = ~USB_ISTR_WKUP;
		USB->CNTR = USB_CNTR_RESETM | USB_CNTR_CTRM | USB_CNTR_SUSPM;
		handleResume();
	}
	if (istr & USB_ISTR_SOF) {
		USB->ISTR = ~USB_ISTR_SOF;
	}
	if (istr & USB_ISTR_ESOF) {
		USB->ISTR = ~USB_ISTR_ESOF;
	}
	if (istr & USB_ISTR_ERR) {
		USB->ISTR = ~USB_ISTR_ERR;
	}
}

ISR(USB_FS_WKUP_vect) {
	if (usbDriver) {
		usbDriver->handleInterruptRequest();
	}
}

ISR(CAN1_RX0_vect) {
	if (usbDriver) {
		usbDriver->handleInterruptRequest();
	}
}

#endif