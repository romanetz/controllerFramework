#include <stm32.h>

void stm32_adcEnable(volatile STM32ADCRegs *adc) {
	stm32_rccPeriphClockEnable(adc);
	adc->CR2 = ADC1_CR2_ADON;
	adc->CR2 |= ADC1_CR2_CAL;
	while (adc->CR2 & ADC1_CR2_CAL);
	adc->CR2 = 0;
}

void stm32_adcDisable(volatile STM32ADCRegs *adc) {
	stm32_rccPeriphClockDisable(adc);
}

void stm32_adcEnableTempSensor(volatile STM32ADCRegs *adc) {
	adc->CR2 |= ADC1_CR2_TSVREFE;
}

void stm32_adcDisableTempSensor(volatile STM32ADCRegs *adc) {
	adc->CR2 &= ~ADC1_CR2_TSVREFE;
}

void stm32_adcStartSingleConversion(volatile STM32ADCRegs *adc, uint8_t channel) {
	adc->CR2 |= (0b111 << ADC1_CR2_EXTSEL_OFFSET) | ADC1_CR2_EXTTRIG | ADC1_CR2_ADON;
	adc->SQR[0] = 0;
	adc->SQR[2] = channel;
	adc->CR2 |= ADC1_CR2_SWSTART;
}

BOOL stm32_adcConversionDone(volatile STM32ADCRegs *adc) {
	return (adc->SR & ADC1_SR_EOC) != 0;
}

void stm32_adcWaitConversionDone(volatile STM32ADCRegs *adc) {
	while (!stm32_adcConversionDone(adc));
}

uint16_t stm32_adcConversionResult(volatile STM32ADCRegs *adc) {
	return adc->DR;
}

#ifdef USE_CLASSES

static adc_value_t STM32ADCClass_singleConversion(ADCClass *_adc, adc_channel_index_t channel) {
	STM32ADCClass *adc = STM32_ADC_CLASS(_adc);
	stm32_adcStartSingleConversion(adc->regs, channel);
	stm32_adcWaitConversionDone(adc->regs);
	return stm32_adcConversionResult(adc->regs);
}

STM32ADCClass *stm32_adcClassInit(STM32ADCClass *adc, volatile STM32ADCRegs *regs) {
	ADC_CLASS(adc)->singleConversion = STM32ADCClass_singleConversion;
	adc->regs = regs;
	stm32_adcEnable(adc->regs);
	return adc;
}

#endif