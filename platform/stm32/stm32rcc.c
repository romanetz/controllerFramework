#include <stdlib.h>
#include <stm32.h>

uint32_t ahbFrequency = 8000000;
uint32_t apb1Frequency = 8000000;
uint32_t apb2Frequency = 8000000;
uint32_t adcFrequency = 4000000;

void stm32_rccEnableClockSource(STM32ClockSource src) {
	switch (src) {
		case STM32_CLOCKSOURCE_HSI:
			RCC->CR |= RCC_CR_HSION;
			break;
		case STM32_CLOCKSOURCE_LSI:
			RCC->CSR |= RCC_CSR_LSION;
			break;
		case STM32_CLOCKSOURCE_HSE:
			RCC->CR |= RCC_CR_HSEON;
			break;
		case STM32_CLOCKSOURCE_LSE:
			RCC->BDCR |= RCC_BDCR_LSEON;
			break;
		case STM32_CLOCKSOURCE_PLL:
			RCC->CR |= RCC_CR_PLLON;
			break;
	}
}

void stm32_rccDisableClockSource(STM32ClockSource src) {
	switch (src) {
		case STM32_CLOCKSOURCE_HSI:
			RCC->CR &= ~RCC_CR_HSION;
			break;
		case STM32_CLOCKSOURCE_LSI:
			RCC->CSR &= ~RCC_CSR_LSION;
			break;
		case STM32_CLOCKSOURCE_HSE:
			RCC->CR &= ~RCC_CR_HSEON;
			break;
		case STM32_CLOCKSOURCE_LSE:
			RCC->BDCR &= ~RCC_BDCR_LSEON;
			break;
		case STM32_CLOCKSOURCE_PLL:
			RCC->CR &= ~RCC_CR_PLLON;
			break;
	}
}

BOOL stm32_rccWaitClockSourceReady(STM32ClockSource src) {
	return stm32_rccWaitClockSourceReadyTimeout(src, 10000);
}

BOOL stm32_rccWaitClockSourceReadyTimeout(STM32ClockSource src, uint32_t timeout) {
	volatile uint32_t *reg = NULL;
	uint32_t flag = 0;
	switch (src) {
		case STM32_CLOCKSOURCE_HSI:
			reg = &RCC->CR;
			flag = RCC_CR_HSIRDY;
			break;
		case STM32_CLOCKSOURCE_LSI:
			reg = &RCC->CSR;
			flag = RCC_CSR_LSIRDY;
			break;
		case STM32_CLOCKSOURCE_HSE:
			reg = &RCC->CR;
			flag = RCC_CR_HSERDY;
			break;
		case STM32_CLOCKSOURCE_LSE:
			reg = &RCC->BDCR;
			flag = RCC_BDCR_LSERDY;
			break;
		case STM32_CLOCKSOURCE_PLL:
			reg = &RCC->CR;
			flag = RCC_CR_PLLRDY;
			break;
	}
	while (((*reg & flag) == 0) && (timeout--));
	return timeout > 0;
}

void stm32_rccSelectClockSource(STM32ClockSource src) {
	uint32_t srcValue;
	switch (src) {
		case STM32_CLOCKSOURCE_PLL:
			srcValue = 2;
			break;
		case STM32_CLOCKSOURCE_HSE:
			srcValue = 1;
			break;
		case STM32_CLOCKSOURCE_HSI:
		default:
			srcValue = 0;
	}
#ifdef RCC_CFGR_SW_MASK
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW_MASK) | srcValue;
#else
	RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_SW0 | RCC_CFGR_SW1)) | srcValue;
#endif
}

void stm32_rccWaitClockSourceSelected(STM32ClockSource src) {
	uint32_t srcValue;
	switch (src) {
		case STM32_CLOCKSOURCE_PLL:
			srcValue = 2;
			break;
		case STM32_CLOCKSOURCE_HSE:
			srcValue = 1;
			break;
		case STM32_CLOCKSOURCE_HSI:
		default:
			srcValue = 0;
	}
#ifdef STM32F1
	while ((RCC->CFGR & RCC_CFGR_SW_MASK) != srcValue);
#else
	while ((RCC->CFGR & (RCC_CFGR_SW0 | RCC_CFGR_SW1)) != srcValue);
#endif
}

void stm32_rccSetAHBPrescaler(STM32AHBPrescaler psc) {
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE_MASK) | (psc << RCC_CFGR_HPRE_OFFSET);
}

void stm32_rccSetAPB1Prescaler(STM32APBPrescaler psc) {
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1_MASK) | (psc << RCC_CFGR_PPRE1_OFFSET);
}

void stm32_rccSetAPB2Prescaler(STM32APBPrescaler psc) {
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE2_MASK) | (psc << RCC_CFGR_PPRE2_OFFSET);
}

#ifdef STM32F1
void stm32_rccSetADCPrescaler(STM32ADCPrescaler psc) {
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_ADCPRE_MASK) | (psc << RCC_CFGR_ADCPRE_OFFSET);
}
#endif

uint32_t stm32_rccAHBFrequency(void) {
	return ahbFrequency;
}

uint32_t stm32_rccAPB1Frequency(void) {
	return apb1Frequency;
}

uint32_t stm32_rccAPB2Frequency(void) {
	return apb2Frequency;
}

#ifdef STM32F1
uint32_t stm32_rccADCFrequency(void) {
	return adcFrequency;
}
#endif

void stm32_rccUseHSI(void) {
	stm32_rccEnableClockSource(STM32_CLOCKSOURCE_HSI);
	stm32_rccWaitClockSourceReady(STM32_CLOCKSOURCE_HSI);
	stm32_rccSelectClockSource(STM32_CLOCKSOURCE_HSI);
	stm32_rccWaitClockSourceSelected(STM32_CLOCKSOURCE_HSI);
	FLASH_ACR = FLASH_ACR & ~FLASH_ACR_LATENCY_MASK;
	stm32_rccSetAHBPrescaler(RCC_AHB_NODIV);
	stm32_rccSetAPB1Prescaler(RCC_APB_NODIV);
	stm32_rccSetAPB2Prescaler(RCC_APB_NODIV);
	ahbFrequency = 8000000;
	apb1Frequency = ahbFrequency;
	apb2Frequency = ahbFrequency;
#ifdef STM32F1
	stm32_rccSetADCPrescaler(STM32_ADCPRESCALER_DIV2);
	adcFrequency = ahbFrequency / 2;
#endif
}

BOOL stm32_rccUseHSE(uint32_t freq) {
	stm32_rccEnableClockSource(STM32_CLOCKSOURCE_HSE);
	if (stm32_rccWaitClockSourceReady(STM32_CLOCKSOURCE_HSE)) {
		stm32_rccSelectClockSource(STM32_CLOCKSOURCE_HSE);
		stm32_rccWaitClockSourceSelected(STM32_CLOCKSOURCE_HSE);
		stm32_rccSetAHBPrescaler(RCC_AHB_NODIV);
		stm32_rccSetAPB1Prescaler(RCC_APB_NODIV);
		stm32_rccSetAPB2Prescaler(RCC_APB_NODIV);
		ahbFrequency = freq;
		FLASH_ACR = (FLASH_ACR & ~FLASH_ACR_LATENCY_MASK) | ((freq - 1) / 24000000);
		apb1Frequency = freq;
		apb2Frequency = freq;
#ifdef STM32F1
		stm32_rccSetADCPrescaler(STM32_ADCPRESCALER_DIV2);
		adcFrequency = freq / 2;
#endif
		return 1;
	}
	return 0;
}

#ifdef STM32F1
BOOL stm32_rccSetupPLL(STM32ClockSource src, uint32_t inClk, uint32_t outClk) {
	if (inClk && outClk) {
		uint32_t cfg = (src == STM32_CLOCKSOURCE_HSE) ? RCC_CFGR_PLLSRC : 0;
		uint32_t mul = outClk / inClk;
		if (src == STM32_CLOCKSOURCE_HSI) {
			mul *= 2;
		}
		if (mul && (mul <= 15)) {
			if (mul < 8) {
				cfg |= RCC_CFGR_PLLXTPRE;
				mul *= 2;
			}
			cfg |= (mul - 2) << RCC_CFGR_PLLMUL_OFFSET;
			if (outClk <= 48000000) {
				cfg |= RCC_CFGR_OTGFSPRE;
			}
			RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_PLLMUL_MASK | RCC_CFGR_PLLXTPRE | RCC_CFGR_OTGFSPRE)) | cfg;
			return 1;
		}
	}
	return 0;
}
#else
BOOL stm32_rccLowlevelSetupPLL(STM32ClockSource src, uint32_t pllm, uint32_t plln, uint32_t pllp, uint32_t pllq) {
	if ((pllm >= 1) && (pllm <= 31) && (plln >= 1) && (plln <= 511) && (pllp >= 2) && (pllp <= 8) && (pllq >= 1) && (pllq <= 31)) {
		uint32_t cfg = (src == STM32_CLOCKSOURCE_HSE) ? RCC_PLLCFGR_PLLSRC : 0;
		cfg |= pllm << 0;
		cfg |= plln << 6;
		cfg |= ((pllp >> 1) - 1) << 16;
		cfg |= pllq << 24;
		RCC->PLLCFGR = cfg;
		return 1;
	}
	return 0;
}
		
BOOL stm32_rccSetupPLL(STM32ClockSource src, uint32_t inClk, uint32_t outClk) {
	if (((inClk % 1000000) == 0) && ((outClk % 1000000) == 0)) {
		uint32_t pllm = inClk / 1000000;
		uint32_t plln, pllp;
		for (pllp = 2; pllp <= 8; pllp += 2) {
			plln = outClk / 1000000 * pllp;
			if ((plln % 48) == 0) {
				break;
			}
		}
		if (plln % 48) {
			pllp = 2;
			plln = outClk / 1000000 * pllp;
		}
		uint32_t pllq = plln / 48;
		return stm32_rccLowlevelSetupPLL(src, pllm, plln, pllp, pllq);
	}
	return 0;
}
#endif

BOOL stm32_rccUsePLL(STM32ClockSource src, uint32_t srcFreq, uint32_t pllFreq) {
	if ((src != STM32_CLOCKSOURCE_HSE) && (src != STM32_CLOCKSOURCE_HSI)) return 0;
	stm32_rccUseHSI();
	stm32_rccEnableClockSource(src);
	if (stm32_rccWaitClockSourceReady(src)) {
		if (stm32_rccSetupPLL(src, srcFreq, pllFreq)) {
			stm32_rccEnableClockSource(STM32_CLOCKSOURCE_PLL);
			if (stm32_rccWaitClockSourceReady(STM32_CLOCKSOURCE_PLL)) {
				stm32_rccSetAHBPrescaler(RCC_AHB_NODIV);
				ahbFrequency = pllFreq;
#ifdef STM32F1
				if (pllFreq <= 28000000) {
					stm32_rccSetADCPrescaler(STM32_ADCPRESCALER_DIV2);
					adcFrequency = pllFreq / 2;
				} else if (pllFreq <= 56000000) {
					stm32_rccSetADCPrescaler(STM32_ADCPRESCALER_DIV4);
					adcFrequency = pllFreq / 4;
				} else if (pllFreq <= 84000000) {
					stm32_rccSetADCPrescaler(STM32_ADCPRESCALER_DIV6);
					adcFrequency = pllFreq / 6;
				} else {
					stm32_rccSetADCPrescaler(STM32_ADCPRESCALER_DIV8);
					adcFrequency = pllFreq / 8;
				}
				if (pllFreq <= 36000000) {
					stm32_rccSetAPB1Prescaler(RCC_APB_NODIV);
					apb1Frequency = pllFreq;
				} else {
					stm32_rccSetAPB1Prescaler(RCC_APB_DIV2);
					apb1Frequency = pllFreq;
				}
				stm32_rccSetAPB2Prescaler(RCC_APB_NODIV);
				apb2Frequency = pllFreq;
#else
				stm32_rccSetAPB1Prescaler(RCC_APB_DIV4);
				stm32_rccSetAPB2Prescaler(RCC_APB_DIV2);
				apb1Frequency = pllFreq / 4;
				apb2Frequency = pllFreq / 2;
#endif
#ifdef STM32F1
				FLASH_ACR = (FLASH_ACR & ~FLASH_ACR_LATENCY_MASK) | FLASH_ACR_PRFTBE | ((pllFreq - 1) / 24000000);
#else
				FLASH_ACR = (FLASH_ACR & ~FLASH_ACR_LATENCY_MASK) | ((pllFreq - 1) / 24000000);
#endif
				stm32_rccSelectClockSource(STM32_CLOCKSOURCE_PLL);
				stm32_rccWaitClockSourceSelected(STM32_CLOCKSOURCE_PLL);
				stm32_rccDisableClockSource(STM32_CLOCKSOURCE_HSI);
				return 1;
			} else {
				stm32_rccDisableClockSource(STM32_CLOCKSOURCE_PLL);
			}
		}
	}
	stm32_rccDisableClockSource(STM32_CLOCKSOURCE_HSE);
	return 0;
}

void stm32_rccPeriphClockEnable(volatile void *periph) {
	const STM32PeriphInfo *info = stm32SearchPeriph(periph);
	if (info) {
		*info->clkEnReg |= info->clkEnMask;
	}
}

void stm32_rccPeriphClockDisable(volatile void *periph) {
	const STM32PeriphInfo *info = stm32SearchPeriph(periph);
	if (info) {
		*info->clkEnReg &= ~info->clkEnMask;
	}
}