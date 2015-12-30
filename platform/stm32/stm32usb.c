#include <stm32.h>

#ifdef USB_BASE

static STM32USBDriverClass *usbDriver;

static void stm32_usbSetAddress(uint8_t addr) {
	USB->DADDR = addr | USB_DADDR_EF;
}

static volatile STM32USBBufferDesc* stm32_usbBufferTable() {
	return (volatile STM32USBBufferDesc*)(USB_PMA_BASE + USB->BTABLE * 2);
}

static void stm32_usbCopyToBufferArea(uint32_t offset, const void *data, int count) {
	count = (count + 1) / 2;
	uint32_t *dst = (uint32_t*)(USB_PMA_BASE + offset * 2);
	const uint16_t *src = (const uint16_t*)data;
	while(count--) {
		*(dst++) = *(src++);
	}
}

static void stm32_usbCopyFromBufferArea(void *ptr, uint32_t offset, int count) {
	count = (count + 1) / 2;
	uint16_t *dst = (uint16_t*)ptr;
	const uint32_t *src = (uint32_t*)(USB_PMA_BASE + offset * 2);
	while(count--) {
		*(dst++) = *(src++);
	}
}

static STM32USBEpStatus stm32_usbEpTxStatus(uint8_t ep) {
	return (STM32USBEpStatus)((USB->EPR[ep] & USB_EP0R_STAT_TX_MASK) >> USB_EP0R_STAT_TX_OFFSET);
}

static void stm32_usbEpSetTxStatus(uint8_t ep, STM32USBEpStatus status) {
	uint32_t epr = USB->EPR[ep];
	epr &= ~(USB_EP0R_STAT_RX_MASK | USB_EP0R_DTOG_RX | USB_EP0R_STAT_TX_MASK | USB_EP0R_DTOG_TX | USB_EP0R_CTR_TX);
	epr |= USB_EP0R_CTR_RX;
	epr |= (USB->EPR[ep] & USB_EP0R_STAT_TX_MASK) ^ (status << USB_EP0R_STAT_TX_OFFSET);
	USB->EPR[ep] = epr;
}

static STM32USBEpStatus stm32_usbEpRxStatus(uint8_t ep) {
	return (STM32USBEpStatus)((USB->EPR[ep] & USB_EP0R_STAT_RX_MASK) >> USB_EP0R_STAT_RX_OFFSET);
}

static void stm32_usbEpSetRxStatus(uint8_t ep, STM32USBEpStatus status) {
	uint32_t epr = USB->EPR[ep];
	epr &= ~(USB_EP0R_STAT_RX_MASK | USB_EP0R_DTOG_RX | USB_EP0R_STAT_TX_MASK | USB_EP0R_DTOG_TX | USB_EP0R_CTR_RX);
	epr |= USB_EP0R_CTR_TX;
	epr |= (USB->EPR[ep] & USB_EP0R_STAT_RX_MASK) ^ (status << USB_EP0R_STAT_RX_OFFSET);
	USB->EPR[ep] = epr;
}

static uint8_t stm32_usbEpReady(uint8_t ep) {
	uint8_t dir = ep & USB_ENDPOINT_IN;
	ep &= 0x7F;
	if (dir) {
		return stm32_usbEpTxStatus(ep) != STM32USB_EP_VALID;
	} else {
		return stm32_usbEpRxStatus(ep) != STM32USB_EP_VALID;
	}
}

static void stm32_usbEpStall(uint8_t ep) {
	uint8_t dir = ep & USB_ENDPOINT_IN;
	ep &= 0x7F;
	if (dir) {
		stm32_usbEpSetTxStatus(ep, STM32USB_EP_STALL);
	} else {
		stm32_usbEpSetRxStatus(ep, STM32USB_EP_STALL);
	}
}

static void stm32_usbEpNAK(uint8_t ep) {uint8_t dir = ep & USB_ENDPOINT_IN;
	ep &= 0x7F;
	if (dir) {
		stm32_usbEpSetTxStatus(ep, STM32USB_EP_NAK);
	} else {
		stm32_usbEpSetRxStatus(ep, STM32USB_EP_NAK);
	}
}

static void stm32_usbEpStartTx(uint8_t ep) {
	ep &= 0x7F;
	stm32_usbEpSetTxStatus(ep, STM32USB_EP_VALID);
}

static void stm32_usbEpStartRx(uint8_t ep) {
	ep &= 0x7F;
	stm32_usbEpSetRxStatus(ep, STM32USB_EP_VALID);
}

static void STM32USBDriverClass_setAddress(USBDriverClass *usb, uint8_t addr) {
	stm32_usbSetAddress(addr);
}

static uint8_t STM32USBDriverClass_epSetup(USBDriverClass *_usb, uint8_t ep, USBEndpointType type, uint16_t bufSize, USBEndpointCallback callback, void *arg) {
	STM32USBDriverClass *usb = STM32_USB_DRIVER_CLASS(_usb);
	uint8_t dir = ep & USB_ENDPOINT_IN;
	ep &= 0x7F;
	if (ep >= STM32USB_MAX_EP_COUNT) return 0;
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
	volatile STM32USBBufferDesc* bufferDesc = stm32_usbBufferTable() + ep;
	bufSize = (bufSize + 1) & ~1;
	if ((dir) || (ep == 0)) {
		usb->epInfo[ep].inCallback = callback;
		usb->epInfo[ep].inCallbackArg = arg;
		usb->epInfo[ep].txBufSize = bufSize;
		bufferDesc->txAddr = usb->pmaFreeOffset;
		usb->pmaFreeOffset += bufSize;
	}
	if (!dir) {
		usb->epInfo[ep].outCallback = callback;
		usb->epInfo[ep].outCallbackArg = arg;
		if (bufSize <= 62) {
			bufferDesc->rxCount = (bufSize / 2) << 10;
		} else {
			bufSize = (bufSize + 31) & ~31;
			bufferDesc->rxCount = 0x8000 | ((bufSize / 32) - 1) << 10;
		}
		bufferDesc->rxAddr = usb->pmaFreeOffset;
		usb->pmaFreeOffset += bufSize;
	}
	uint32_t origEpr = USB->EPR[ep];
	uint32_t epr = origEpr &
		~(USB_EP0R_STAT_RX_MASK | USB_EP0R_DTOG_RX | USB_EP0R_STAT_TX_MASK | USB_EP0R_DTOG_TX | USB_EP0R_EP_TYPE_MASK | USB_EP0R_EA_MASK);
	if ((dir) || (ep == 0)) {
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
	return 1;
}

void STM32USBDriverClass_epStall(USBDriverClass *_usb, uint8_t ep) {
	stm32_usbEpStall(ep);
}

void STM32USBDriverClass_epNAK(USBDriverClass *_usb, uint8_t ep) {
	stm32_usbEpNAK(ep);
}

int STM32USBDriverClass_epWrite(USBDriverClass *_usb, uint8_t ep, const void *data, int count) {
	STM32USBDriverClass *usb = STM32_USB_DRIVER_CLASS(_usb);
	if ((ep & USB_ENDPOINT_IN) == 0) return -1;
	if (!stm32_usbEpReady(ep)) return 0;
	ep &= 0x7F;
	if (count > usb->epInfo[ep].txBufSize) count = usb->epInfo[ep].txBufSize;
	volatile STM32USBBufferDesc *bufferDesc = stm32_usbBufferTable() + ep;
	stm32_usbCopyToBufferArea(bufferDesc->txAddr, data, count);
	bufferDesc->txCount = count;
	return count;
}

void STM32USBDriverClass_epStartTx(USBDriverClass *_usb, uint8_t ep) {
	stm32_usbEpStartTx(ep);
}

int STM32USBDriverClass_epRead(USBDriverClass *_usb, uint8_t ep, void *buffer, int count) {
	//STM32USBDriverClass *usb = STM32_USB_DRIVER_CLASS(_usb);
	if (ep & USB_ENDPOINT_IN) return -1;
	if (!stm32_usbEpReady(ep)) return 0;
	volatile STM32USBBufferDesc *bufferDesc = stm32_usbBufferTable() + ep;
	uint16_t len = bufferDesc->rxCount & 0x3FF;
	if (len > count) len = count;
	stm32_usbCopyFromBufferArea(buffer, bufferDesc->rxAddr, len);
	return len;
}

void STM32USBDriverClass_epStartRx(USBDriverClass *_usb, uint8_t ep) {
	stm32_usbEpStartRx(ep);
}

uint8_t STM32USBDriverClass_epReady(USBDriverClass *_usb, uint8_t ep) {
	return stm32_usbEpReady(ep);
}

STM32USBDriverClass *stm32_usbDriverClassInit(STM32USBDriverClass *usb, const USBDeviceDescriptor *deviceDescr, const USBConfigDescriptor **configs,
		const USBStringDescriptor **strings, int stringCount) {
	USB_DRIVER_CLASS(usb)->setAddress = STM32USBDriverClass_setAddress;
	USB_DRIVER_CLASS(usb)->epSetup = STM32USBDriverClass_epSetup;
	USB_DRIVER_CLASS(usb)->epStall = STM32USBDriverClass_epStall;
	USB_DRIVER_CLASS(usb)->epNAK = STM32USBDriverClass_epNAK;
	USB_DRIVER_CLASS(usb)->epWrite = STM32USBDriverClass_epWrite;
	USB_DRIVER_CLASS(usb)->epStartTx = STM32USBDriverClass_epStartTx;
	USB_DRIVER_CLASS(usb)->epRead = STM32USBDriverClass_epRead;
	USB_DRIVER_CLASS(usb)->epStartRx = STM32USBDriverClass_epStartRx;
	USB_DRIVER_CLASS(usb)->epReady = STM32USBDriverClass_epReady;
	USB_DRIVER_CLASS(usb)->earlySetAddress = 0;
	usbDriverClassInit(usb, deviceDescr, configs, strings, stringCount);
	usbDriver = usb;
	stm32_rccPeriphClockEnable(USB);
	USB->CNTR = USB_CNTR_FRES;
	usleep(1);
	USB->CNTR &= ~USB_CNTR_FRES;
	USB->BTABLE = 0;
	USB->ISTR = 0;
	nvicEnableIRQ(USB_FS_WKUP_IRQ);
	nvicEnableIRQ(CAN1_RX0_IRQ);
	USB->CNTR = USB_CNTR_RESETM;
	return usb;
}

static void STM32USBDriver_handleInterrupt(STM32USBDriverClass *usb) {
	uint16_t istr = USB->ISTR;
	if (istr & USB_ISTR_RESET) {
		USB->BTABLE = 0;
		USB->ISTR = 0;
		usb->pmaFreeOffset = 8 * STM32USB_MAX_EP_COUNT;
		usbDriverHandleReset(usb);
		USB->CNTR = USB_CNTR_RESETM | USB_CNTR_CTRM | USB_CNTR_SUSPM;
		return;
	}
	if (istr & USB_ISTR_CTR) {
		uint8_t ep = istr & USB_ISTR_EP_ID_MASK;
		if (istr & USB_ISTR_DIR) {
			if (usb->epInfo[ep].outCallback) {
				usb->epInfo[ep].outCallback(USB_DRIVER_CLASS(usb), ep | USB_ENDPOINT_OUT, (USB->EPR[ep] & USB_EP0R_SETUP) != 0,
					usb->epInfo[ep].outCallbackArg);
			}
		} else {
			if (usb->epInfo[ep].inCallback) {
				usb->epInfo[ep].inCallback(USB_DRIVER_CLASS(usb), ep | USB_ENDPOINT_IN, 0, usb->epInfo[ep].inCallbackArg);
			}
		}
		uint32_t epr = USB->EPR[ep];
		epr &= ~(USB_EP0R_STAT_RX_MASK | USB_EP0R_DTOG_RX | USB_EP0R_STAT_TX_MASK | USB_EP0R_DTOG_TX | USB_EP0R_CTR_TX | USB_EP0R_CTR_RX);
		if (istr & USB_ISTR_DIR) {
			if (!usb->epInfo[ep].outCallback) {
				epr |= USB_EP0R_CTR_TX; // Clear RX flag
				USB->EPR[ep] = epr;
			}
		} else {
			if (!usb->epInfo[ep].inCallback) {
				epr |= USB_EP0R_CTR_RX; // Clear TX flag
				USB->EPR[ep] = epr;
			}
		}
	}
	if ((istr & USB_ISTR_SUSP) && (USB_DRIVER_CLASS(usb)->state != USB_DRIVER_SUSPEND)) {
		USB->ISTR = ~USB_ISTR_SUSP;
		USB->CNTR = USB->CNTR = USB_CNTR_RESETM | USB_CNTR_CTRM | USB_CNTR_WKUPM | USB_CNTR_FSUSP;
		usbDriverHandleSuspend(usb);
	}
	if ((istr & USB_ISTR_WKUP) && (USB_DRIVER_CLASS(usb)->state == USB_DRIVER_SUSPEND)) {
		USB->ISTR = ~USB_ISTR_WKUP;
		USB->CNTR = USB_CNTR_RESETM | USB_CNTR_CTRM | USB_CNTR_SUSPM;
		usbDriverHandleResume(usb);
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
		STM32USBDriver_handleInterrupt(usbDriver);
	}
}

ISR(CAN1_RX0_vect) {
	if (usbDriver) {
		STM32USBDriver_handleInterrupt(usbDriver);
	}
}

#endif
