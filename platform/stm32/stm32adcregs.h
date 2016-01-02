#ifndef __STM32ADCREGS_H__
#define __STM32ADCREGS_H__

#include <stdint.h>

typedef struct STM32ADCRegs {
	uint32_t SR;
	uint32_t CR1;
	uint32_t CR2;
	uint32_t SMPR[2];
	uint32_t JOFR[4];
	uint32_t HTR;
	uint32_t LTR;
	uint32_t SQR[3];
	uint32_t JSQR;
	uint32_t JDR[4];
	uint32_t DR;
} STM32ADCRegs;

#define ADC1 MMIO(ADC1_BASE, STM32ADCRegs)

#ifdef ADC2_BASE
#define ADC2 MMIO(ADC2_BASE, STM32ADCRegs)
#endif

#ifdef ADC3_BASE
#define ADC3 MMIO(ADC3_BASE, STM32ADCRegs)
#endif

#ifdef STM32F4
#define STM32_ADC_CHANNEL_COUNT 19
#else
#define STM32_ADC_CHANNEL_COUNT 18
#endif

#endif
