#ifndef __STM32USBOTGREGS_H__
#define __STM32USBOTGREGS_H__

#include <stm32.h>

#ifdef OTG_FS_DEVICE_BASE

typedef struct STM32OTGGlobalRegs {
	uint32_t GOTGCTL;
	uint32_t GOTGINT;
	uint32_t GAHBCFG;
	uint32_t GUSBCFG;
	uint32_t GRSTCTL;
	uint32_t GINTSTS;
	uint32_t GINTMSK;
	uint32_t GRXSTSR;
	uint32_t GRXSTSP;
	uint32_t GRXFSIZ;
	uint32_t GNPTXFSIZ;
	uint32_t GNPTXSTS;
	uint32_t RESERVED1[2];
	uint32_t GCCFG;
	uint32_t CID;
	uint32_t RESERVED2[48];
	uint32_t HPTXFSIZ;
	uint32_t DIEPTXF[3]; // 1..3
} STM32OTGGlobalRegs;

typedef struct STM32OTGPowerRegs {
	uint32_t PCGCCTL;
} STM32OTGPowerRegs;

typedef struct STM32OTGEpRegs {
	uint32_t CTL; // 0x00
	uint32_t RESERVED1;
	uint32_t INT; // 0x08
	uint32_t RESERVED2;
	uint32_t SIZE; // 0x10
	uint32_t RESERVED3;
	uint32_t STS; // 0x18
	uint32_t RESERVED4;
} STM32OTGEpRegs;

typedef struct STM32OTGDeviceRegs {
	uint32_t DCFG;
	uint32_t DCTL;
	uint32_t DSTS;
	uint32_t RESERVED0;
	uint32_t DIEPMSK;
	uint32_t DOEPMSK;
	uint32_t DAINT;
	uint32_t DAINTMSK;
	uint32_t RESEVRED1[2];
	uint32_t DVBUSDIS;
	uint32_t DVBUSPULSE;
	uint32_t RESEVRED2;
	uint32_t DIEPEMPMSK;
	uint32_t RESERVED3[50];
	STM32OTGEpRegs DIEP[4];
	uint32_t RESERVED4[96];
	STM32OTGEpRegs DOEP[4];
} STM32OTGDeviceRegs;

#define OTG_FS_GLOBAL MMIO(OTG_FS_GLOBAL_BASE, STM32OTGGlobalRegs)

#define OTG_FS_PWRCLK MMIO(OTG_FS_PWRCLK_BASE, STM32OTGPowerRegs)

#define OTG_FS_DEVICE MMIO(OTG_FS_DEVICE_BASE, STM32OTGDeviceRegs)

typedef enum STM32USBPacketStatus {
	STM32USB_PKTSTS_GLOBAL_OUT_NAK = 1,
	STM32USB_PKTSTS_OUT_RX = 2,
	STM32USB_PKTSTS_OUT_COMPLETED = 3,
	STM32USB_PKTSTS_SETUP_COMPLETED = 4,
	STM32USB_PKTSTS_SETUP_RX = 6
} STM32USBPacketStatus;

#define STM32USB_MAX_EP_COUNT 4

#endif

#endif
