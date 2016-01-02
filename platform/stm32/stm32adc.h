#ifndef __STM32ADC_H__
#define __STM32ADC_H__

#include <generic.h>
#include <adc.h>
#include <stm32adcregs.h>

void stm32_adcEnable(volatile STM32ADCRegs *adc);
void stm32_adcDisable(volatile STM32ADCRegs *adc);

void stm32_adcEnableTempSensor(volatile STM32ADCRegs *adc);
void stm32_adcDisableTempSensor(volatile STM32ADCRegs *adc);

void stm32_adcStartSingleConversion(volatile STM32ADCRegs *adc, uint8_t channel);
BOOL stm32_adcConversionDone(volatile STM32ADCRegs *adc);
void stm32_adcWaitConversionDone(volatile STM32ADCRegs *adc);
uint16_t stm32_adcConversionResult(volatile STM32ADCRegs *adc);

#ifdef USE_CLASSES

typedef struct STM32ADCClass STM32ADCClass;

struct STM32ADCClass {
	ADCClass parent;
	volatile STM32ADCRegs *regs;
};

#define STM32_ADC_CLASS(obj) ((STM32ADCClass*)(obj))

STM32ADCClass *stm32_adcClassInit(STM32ADCClass *adc, volatile STM32ADCRegs *regs);

#endif

#endif
