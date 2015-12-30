#ifndef __STM32_PERIPHINFO_H__
#define __STM32_PERIPHINFO_H__

#include <stm32.h>

typedef struct STM32PeriphInfo {
	uint32_t base;
	volatile uint32_t *clkEnReg;
	uint32_t clkEnMask;
} STM32PeriphInfo;

const STM32PeriphInfo *stm32SearchPeriph(volatile void *periph);

#endif
