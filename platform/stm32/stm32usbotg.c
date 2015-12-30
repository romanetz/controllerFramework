#include <stm32.h>

#ifdef OTG_FS_DEVICE_BASE

extern STM32USARTClass serialPort;

static STM32USBDriverClass *otgFSDriver;

static void STM32USBDriverClass_resetFIFO(STM32USBDriverClass *usb) {
	usb->fifoTop = 0;
}

static uint16_t STM32USBDriverClass_allocFIFO(STM32USBDriverClass *usb, uint16_t count) {
	uint16_t r = usb->fifoTop;
	usb->fifoTop += count;
	return r;
}

static void STM32USBDriverClass_setAddress(USBDriverClass *_usb, uint8_t addr) {
	STM32USBDriverClass *usb = STM32_USB_DRIVER_CLASS(_usb);
	ioStreamPrintf(&serialPort, "setAddress(%i)\r\n", addr);
	usb->deviceRegs->DCFG = (usb->deviceRegs->DCFG & ~OTG_FS_DEVICE_FS_DCFG_DAD_MASK) | ((uint32_t)addr << OTG_FS_DEVICE_FS_DCFG_DAD_OFFSET);
}

static uint8_t STM32USBDriverClass_epSetup(USBDriverClass *_usb, uint8_t ep, USBEndpointType type, uint16_t bufSize, USBEndpointCallback callback, void *arg) {
	STM32USBDriverClass *usb = STM32_USB_DRIVER_CLASS(_usb);
	ioStreamPrintf(&serialPort, "epSetup(%i)\r\n", ep);
	uint8_t dir = ep & 0x80;
	ep &= ~0x80;
	if (ep >= STM32USB_MAX_EP_COUNT) return 0;
	if (ep == 0) {
		bufSize = (bufSize + 3) & ~3;
		uint32_t mpsiz;
		if (bufSize >= 64) {
			mpsiz = 0;
		} else if (bufSize >= 32) {
			mpsiz = 1;
		} else if (bufSize >= 16) {
			mpsiz = 2;
		} else {
			mpsiz = 3;
		}
		usb->deviceRegs->DOEP[0].SIZE = 0;
		usb->deviceRegs->DOEP[0].CTL = mpsiz << OTG_FS_DEVICE_DOEPCTL0_MPSIZ_OFFSET;
		usb->deviceRegs->DIEP[0].SIZE = 0;
		usb->deviceRegs->DIEP[0].CTL = (mpsiz << OTG_FS_DEVICE_FS_DIEPCTL0_MPSIZ_OFFSET) | (0 << OTG_FS_DEVICE_FS_DIEPCTL0_TXFNUM_OFFSET);
		usb->globalRegs->HPTXFSIZ = (STM32USBDriverClass_allocFIFO(usb, bufSize / 4) << OTG_FS_GLOBAL_FS_HPTXFSIZ_PTXSA_OFFSET) |
			((bufSize / 4) << OTG_FS_GLOBAL_FS_HPTXFSIZ_PTXFSIZ_OFFSET);
		usb->epInfo[ep].rxBufSize = (3 << OTG_FS_DEVICE_DOEPTSIZ0_STUPCNT_OFFSET) | OTG_FS_DEVICE_DOEPTSIZ0_PKTCNT | bufSize;
		usb->epInfo[ep].txBufSize = bufSize;
		usb->epInfo[ep].outCallback = callback;
		usb->epInfo[ep].outCallbackArg = arg;
		usb->epInfo[ep].inCallback = callback;
		usb->epInfo[ep].inCallbackArg = arg;
		usb->deviceRegs->DAINTMSK |= (1 << OTG_FS_DEVICE_FS_DAINTMSK_IEPM_OFFSET) | (1 << OTG_FS_DEVICE_FS_DAINTMSK_OEPINT_OFFSET);
		return 1;
	}
	return 0;
	volatile STM32OTGEpRegs *epRegs = (dir ? usb->deviceRegs->DIEP : usb->deviceRegs->DOEP) + ep;
	if (dir) {
		bufSize = (bufSize + 3) & ~3;
		usb->globalRegs->DIEPTXF[ep - 1] = (usb->fifoTop << OTG_FS_GLOBAL_FS_DIEPTXF1_INEPTXSA_OFFSET) |
			((bufSize / 4) << OTG_FS_GLOBAL_FS_DIEPTXF1_INEPTXFD_OFFSET);
		usb->fifoTop += bufSize;
		epRegs->CTL = OTG_FS_DEVICE_FS_DIEPCTL0_SNAK | (type << OTG_FS_DEVICE_FS_DIEPCTL0_EPTYP_OFFSET) | OTG_FS_DEVICE_FS_DIEPCTL0_USBAEP;
		usb->epInfo[ep].txBufSize = bufSize;
		usb->epInfo[ep].inCallback = callback;
		usb->epInfo[ep].inCallbackArg = arg;
	} else {
		epRegs->CTL = OTG_FS_DEVICE_FS_DIEPCTL0_SNAK | (type << OTG_FS_DEVICE_FS_DIEPCTL0_EPTYP_OFFSET) | OTG_FS_DEVICE_FS_DIEPCTL0_USBAEP;
		usb->epInfo[ep].rxBufSize = OTG_FS_DEVICE_DOEPTSIZ0_PKTCNT | bufSize;
		usb->epInfo[ep].outCallback = callback;
		usb->epInfo[ep].outCallbackArg = arg;
	}
	return 1;
}

void STM32USBDriverClass_epStall(USBDriverClass *_usb, uint8_t ep) {
	STM32USBDriverClass *usb = STM32_USB_DRIVER_CLASS(_usb);
	uint8_t dir = ep & 0x80;
	ep &= ~0x80;
	volatile STM32OTGEpRegs *epRegs = (dir ? usb->deviceRegs->DIEP : usb->deviceRegs->DOEP) + ep;
	epRegs->CTL |= OTG_FS_DEVICE_FS_DIEPCTL0_STALL;
}

void STM32USBDriverClass_epNAK(USBDriverClass *_usb, uint8_t ep) {
	STM32USBDriverClass *usb = STM32_USB_DRIVER_CLASS(_usb);
	uint8_t dir = ep & 0x80;
	ep &= ~0x80;
	volatile STM32OTGEpRegs *epRegs = (dir ? usb->deviceRegs->DIEP : usb->deviceRegs->DOEP) + ep;
	epRegs->CTL |= OTG_FS_DEVICE_FS_DIEPCTL0_SNAK;
}

static void STM32USBDriverClass_writeTxFIFO(STM32USBDriverClass *usb, uint8_t ep, uint32_t data) {
	volatile uint32_t *reg = (volatile uint32_t*)((uint32_t)usb->globalRegs + (ep + 1) * 4096);
	*reg = data;
}
		
static uint32_t STM32USBDriverClass_readRxFIFO(STM32USBDriverClass *usb, uint8_t ep) {
	volatile uint32_t *reg = (volatile uint32_t*)((uint32_t)usb->globalRegs + (ep + 1) * 4096);
	uint32_t data = *reg;
	usb->rxCount -= 4;
	return data;
}

int STM32USBDriverClass_epWrite(USBDriverClass *_usb, uint8_t ep, const void *data, int count) {
	STM32USBDriverClass *usb = STM32_USB_DRIVER_CLASS(_usb);
	uint8_t dir = ep & 0x80;
	if (!dir) return -1;
	ep &= ~0x80;
	if (count > usb->epInfo[ep].txBufSize) count = usb->epInfo[ep].txBufSize;
	volatile STM32OTGEpRegs *epRegs = (dir ? usb->deviceRegs->DIEP : usb->deviceRegs->DOEP) + ep;
	const uint32_t *data32 = (const uint32_t*)data;
	for (int i = 0; i < count / 4; i++) {
		STM32USBDriverClass_writeTxFIFO(usb, ep, *(data32++));
	}
	epRegs->SIZE = (1 << OTG_FS_DEVICE_DIEPTSIZ0_PKTCNT_OFFSET) | count;
	return count;
}

void STM32USBDriverClass_epStartTx(USBDriverClass *_usb, uint8_t ep) {
	STM32USBDriverClass *usb = STM32_USB_DRIVER_CLASS(_usb);
	uint8_t dir = ep & 0x80;
	if (!dir) return;
	ep &= ~0x80;
	volatile STM32OTGEpRegs *epRegs = (dir ? usb->deviceRegs->DIEP : usb->deviceRegs->DOEP) + ep;
	epRegs->CTL = (epRegs->CTL & ~OTG_FS_DEVICE_FS_DIEPCTL0_STALL) | OTG_FS_DEVICE_FS_DIEPCTL0_EPENA | OTG_FS_DEVICE_FS_DIEPCTL0_CNAK;
}

int STM32USBDriverClass_epRead(USBDriverClass *_usb, uint8_t ep, void *buffer, int bufferSize) {
	STM32USBDriverClass *usb = STM32_USB_DRIVER_CLASS(_usb);
	int count = usb->rxCount;
	if (count > bufferSize) count = bufferSize;
	uint32_t *buf32 = (uint32_t*)buffer;
	int i;
	for (i = count; i >= 4; i -= 4) {
		*(buf32++) = STM32USBDriverClass_readRxFIFO(usb, ep);
	}
	if (i > 0) {
		uint32_t extra = STM32USBDriverClass_readRxFIFO(usb, ep);
		memcpy(buf32, &extra, i);
	}
	return count;
}

void STM32USBDriverClass_epStartRx(USBDriverClass *_usb, uint8_t ep) {
	STM32USBDriverClass *usb = STM32_USB_DRIVER_CLASS(_usb);
	ioStreamPrintf(&serialPort, "epStartRx(%i)\r\n", ep);
	uint8_t dir = ep & 0x80;
	if (dir) return;
	ep &= ~0x80;
	volatile STM32OTGEpRegs *epRegs = (dir ? usb->deviceRegs->DIEP : usb->deviceRegs->DOEP) + ep;
	epRegs->SIZE = usb->epInfo[ep].rxBufSize;
	epRegs->CTL |= OTG_FS_DEVICE_FS_DIEPCTL0_EPENA | OTG_FS_DEVICE_FS_DIEPCTL0_CNAK;
}

uint8_t STM32USBDriverClass_epReady(USBDriverClass *_usb, uint8_t ep) {
	return 0;
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
	USB_DRIVER_CLASS(usb)->earlySetAddress = 1;
	usb->globalRegs = OTG_FS_GLOBAL;
	usb->powerRegs = OTG_FS_PWRCLK;
	usb->deviceRegs = OTG_FS_DEVICE;
	usbDriverClassInit(usb, deviceDescr, configs, strings, stringCount);
	otgFSDriver = usb;
	stm32_rccPeriphClockEnable(usb->globalRegs);
	usb->globalRegs->GINTSTS = 0xFFFFFFFF;
	usb->globalRegs->GUSBCFG |= OTG_FS_GLOBAL_FS_GUSBCFG_PHYSEL;
	usb->globalRegs->GCCFG |= OTG_FS_GLOBAL_FS_GCCFG_PWRDWN | OTG_FS_GLOBAL_FS_GCCFG_VBUSBSEN;
	while ((usb->globalRegs->GRSTCTL & OTG_FS_GLOBAL_FS_GRSTCTL_AHBIDL) == 0);
	usb->globalRegs->GRSTCTL |= OTG_FS_GLOBAL_FS_GRSTCTL_CSRST;
	while (usb->globalRegs->GRSTCTL & OTG_FS_GLOBAL_FS_GRSTCTL_CSRST);
	uint32_t ahbFreq = stm32_rccAHBFrequency();
	uint32_t trdt;
	if (ahbFreq <= 15000000) {
		trdt = 0xF;
	} else if (ahbFreq <= 16000000) {
		trdt = 0xE;
	} else if (ahbFreq <= 17200000) {
		trdt = 0xD;
	} else if (ahbFreq <= 18500000) {
		trdt = 0xC;
	} else if (ahbFreq <= 20000000) {
		trdt = 0xB;
	} else if (ahbFreq <= 21800000) {
		trdt = 0xA;
	} else if (ahbFreq <= 24000000) {
		trdt = 0x9;
	} else if (ahbFreq <= 27500000) {
		trdt = 0x8;
	} else if (ahbFreq <= 32000000) {
		trdt = 0x7;
	} else {
		trdt = 0x6;
	}
	usb->globalRegs->GUSBCFG = (usb->globalRegs->GUSBCFG & ~OTG_FS_GLOBAL_FS_GUSBCFG_TRDT_MASK) | OTG_FS_GLOBAL_FS_GUSBCFG_FDMOD |
		(trdt << OTG_FS_GLOBAL_FS_GUSBCFG_TRDT_OFFSET);
	usb->deviceRegs->DCFG |= 3 << OTG_FS_DEVICE_FS_DCFG_DSPD_OFFSET;
	usb->powerRegs->PCGCCTL = 0;
	usb->globalRegs->GAHBCFG |= OTG_FS_GLOBAL_FS_GAHBCFG_GINT;
	usb->globalRegs->GINTMSK = OTG_FS_GLOBAL_FS_GINTMSK_ENUMDNEM;
	nvicEnableIRQ(OTG_FS_IRQ);
	nvicEnableIRQ(OTG_FS_WKUP_IRQ);
	return usb;
}

static void STM32USBDriverClass_handleReset(STM32USBDriverClass *usb) {
	int i;
	for (i = 0; i < STM32USB_MAX_EP_COUNT; i++) {
		usb->deviceRegs->DIEP[i].CTL = OTG_FS_DEVICE_FS_DIEPCTL0_SNAK;
		usb->deviceRegs->DOEP[i].CTL = OTG_FS_DEVICE_DOEPCTL0_SNAK;
		usb->deviceRegs->DIEP[i].INT = 0xFF;
		usb->deviceRegs->DOEP[i].INT = 0xFF;
		usb->epInfo[i].inCallback = NULL;
		usb->epInfo[i].outCallback = NULL;
	}
	usb->deviceRegs->DAINT = 0xFFFFFFFF;
	usb->deviceRegs->DAINTMSK = 0;
	usb->globalRegs->GINTSTS = 0xFFFFFFFF;
	STM32USBDriverClass_resetFIFO(usb);
	STM32USBDriverClass_allocFIFO(usb, 128);
	usb->globalRegs->GRXFSIZ = 128;
	usb->globalRegs->GINTMSK = OTG_FS_GLOBAL_FS_GINTMSK_ENUMDNEM | OTG_FS_GLOBAL_FS_GINTMSK_RXFLVLM | OTG_FS_GLOBAL_FS_GINTMSK_IEPINT |
		OTG_FS_GLOBAL_FS_GINTMSK_OEPINT | OTG_FS_GLOBAL_FS_GINTMSK_USBSUSPM;
	usb->deviceRegs->DIEPMSK = OTG_FS_DEVICE_FS_DIEPMSK_XFRCM;
	usb->deviceRegs->DOEPMSK = OTG_FS_DEVICE_FS_DOEPMSK_XFRCM | OTG_FS_DEVICE_FS_DOEPMSK_STUPM;
	usbDriverHandleReset(usb);
}

static void STM32USBDriver_handleInterrupt(STM32USBDriverClass *usb) {
	uint32_t gintsts = usb->globalRegs->GINTSTS;
	ioStreamPrintf(&serialPort, "INTERRUPT(%x, %x)\r\n", gintsts, usb->deviceRegs->DOEP[0].INT);
	if (gintsts & OTG_FS_GLOBAL_FS_GINTSTS_ENUMDNE) {
		ioStreamPrintf(&serialPort, "ENUMDNE\r\n");
		STM32USBDriverClass_handleReset(usb);
		return;
	}
	if (gintsts & OTG_FS_GLOBAL_FS_GINTSTS_RXFLVL) {
		ioStreamPrintf(&serialPort, "RXFLVL\r\n");
		uint32_t rxstsp = usb->globalRegs->GRXSTSP;
		uint32_t pktsts = (rxstsp & OTG_FS_GLOBAL_FS_GRXSTSR_Device_PKTSTS_MASK) >> OTG_FS_GLOBAL_FS_GRXSTSR_Device_PKTSTS_OFFSET;
		if ((pktsts != STM32USB_PKTSTS_OUT_COMPLETED) && (pktsts != STM32USB_PKTSTS_SETUP_COMPLETED)) return;
		ioStreamPrintf(&serialPort, "RXFLVL ok\r\n");
		uint32_t count = (rxstsp & OTG_FS_GLOBAL_FS_GRXSTSR_Device_BCNT_MASK) >> OTG_FS_GLOBAL_FS_GRXSTSR_Device_BCNT_OFFSET;
		uint32_t ep = (rxstsp & OTG_FS_GLOBAL_FS_GRXSTSR_Device_EPNUM_MASK) >> OTG_FS_GLOBAL_FS_GRXSTSR_Device_EPNUM_OFFSET;
		uint8_t setup = pktsts == STM32USB_PKTSTS_SETUP_COMPLETED;
		usb->rxCount = count;
		if (usb->epInfo[ep].outCallback) {
			usb->epInfo[ep].outCallback(USB_DRIVER_CLASS(usb), ep, setup, usb->epInfo[ep].outCallbackArg);
		}
		while (usb->rxCount > 0) {
			STM32USBDriverClass_readRxFIFO(usb, ep);
		}
		usb->rxCount = 0;
	}
	for (int ep = 0; ep < STM32USB_MAX_EP_COUNT; ep++) {
		if (usb->deviceRegs->DIEP[ep].INT & OTG_FS_DEVICE_DIEPINT0_XFRC) {
			if (usb->epInfo[ep].inCallback) {
				usb->epInfo[ep].inCallback(USB_DRIVER_CLASS(usb), ep, 0, usb->epInfo[ep].inCallbackArg);
			}
			usb->deviceRegs->DIEP[ep].INT = OTG_FS_DEVICE_DIEPINT0_XFRC;
		}
	}
	if (gintsts & OTG_FS_GLOBAL_FS_GINTSTS_USBSUSP) {
		ioStreamPrintf(&serialPort, "SUSP\r\n");
		usb->globalRegs->GINTSTS = OTG_FS_GLOBAL_FS_GINTSTS_USBSUSP;
		usb->globalRegs->GINTMSK = OTG_FS_GLOBAL_FS_GINTMSK_ENUMDNEM | OTG_FS_GLOBAL_FS_GINTMSK_WUIM;
		usbDriverHandleSuspend(usb);
	}
	if (gintsts & OTG_FS_GLOBAL_FS_GINTSTS_WKUPINT) {
		ioStreamPrintf(&serialPort, "WKUP\r\n");
		usb->globalRegs->GINTSTS = OTG_FS_GLOBAL_FS_GINTSTS_WKUPINT;
		usb->globalRegs->GINTMSK = OTG_FS_GLOBAL_FS_GINTMSK_ENUMDNEM | OTG_FS_GLOBAL_FS_GINTMSK_RXFLVLM | OTG_FS_GLOBAL_FS_GINTMSK_IEPINT |
			OTG_FS_GLOBAL_FS_GINTMSK_USBSUSPM;
		usbDriverHandleResume(usb);
	}
}

ISR(OTG_FS_vect) {
	if (otgFSDriver) {
		STM32USBDriver_handleInterrupt(otgFSDriver);
	}
}

ISR(OTG_FS_WKUP_vect) {
	if (otgFSDriver) {
		STM32USBDriver_handleInterrupt(otgFSDriver);
	}
}

#endif
