#ifndef __STM32USBREGS_H__
#define __STM32USBREGS_H__

#ifdef USB_BASE

typedef struct STM32USBRegs {
	uint32_t EPR[8];
	uint32_t RESERVED[8];
	uint32_t CNTR;
	uint32_t ISTR;
	uint32_t FNR;
	uint32_t DADDR;
	uint32_t BTABLE;
} STM32USBRegs;

typedef struct STM32USBBufferDesc {
	uint32_t txAddr;
	uint32_t txCount;
	uint32_t rxAddr;
	uint32_t rxCount;
} STM32USBBufferDesc;

typedef enum STM32USBEpType {
	STM32USB_EP_BULK = 0,
	STM32USB_EP_CONTROL = 1,
	STM32USB_EP_ISO = 2,
	STM32USB_EP_INTERRUPT = 3
} STM32USBEpType;

typedef enum STM32USBEpStatus {
	STM32USB_EP_DISABLED = 0,
	STM32USB_EP_STALL = 1,
	STM32USB_EP_NAK = 2,
	STM32USB_EP_VALID = 3
} STM32USBEpStatus;

#define STM32USB_MAX_EP_COUNT 8

#define USB_PMA_BASE (USB_BASE + 1024)

#define USB MMIO(USB_BASE, STM32USBRegs)

#endif

#endif
