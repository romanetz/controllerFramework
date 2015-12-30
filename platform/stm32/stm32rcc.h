#ifndef __STM32RCC_H__
#define __STM32RCC_H__

#include <stm32.h>
#include <stm32rccregs.h>

#define RCC MMIO(RCC_BASE, STM32RCCRegs)

typedef enum STM32ClockSource {
	STM32_CLOCKSOURCE_HSI,
	STM32_CLOCKSOURCE_LSI,
	STM32_CLOCKSOURCE_HSE,
	STM32_CLOCKSOURCE_LSE,
	STM32_CLOCKSOURCE_PLL
} STM32ClockSource;

void stm32_rccEnableClockSource(STM32ClockSource src);
void stm32_rccDisableClockSource(STM32ClockSource src);
uint8_t stm32_rccWaitClockSourceReady(STM32ClockSource src);
uint8_t stm32_rccWaitClockSourceReadyTimeout(STM32ClockSource src, uint32_t timeout);
void stm32_rccSelectClockSource(STM32ClockSource src);
void stm32_rccWaitClockSourceSelected(STM32ClockSource src);
void stm32_rccSetAHBPrescaler(STM32AHBPrescaler psc);
void stm32_rccSetAPB1Prescaler(STM32APBPrescaler psc);
void stm32_rccSetAPB2Prescaler(STM32APBPrescaler psc);
#ifdef STM32F1
void stm32_rccSetADCPrescaler(STM32ADCPrescaler psc);
#endif

uint32_t stm32_rccAHBFrequency(void);
uint32_t stm32_rccAPB1Frequency(void);
uint32_t stm32_rccAPB2Frequency(void);
#ifdef STM32F1
uint32_t stm32_rccADCFrequency(void);
#endif

#ifndef STM32F1
uint8_t stm32_rccLowlevelSetupPLL(STM32ClockSource src, uint32_t pllm, uint32_t plln, uint32_t pllp, uint32_t pllq);
#endif
uint8_t stm32_rccSetupPLL(STM32ClockSource src, uint32_t inClk, uint32_t outClk);

void stm32_rccUseHSI(void);
uint8_t stm32_rccUseHSE(uint32_t freq);
uint8_t stm32_rccUsePLL(STM32ClockSource src, uint32_t srcFreq, uint32_t pllFreq);

void stm32_rccPeriphClockEnable(volatile void *periph);
void stm32_rccPeriphClockDisable(volatile void *periph);

#endif
