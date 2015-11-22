#include <stm32.h>

typedef struct PeriphInfo {
	volatile void *base;
	volatile uint32_t *clkEnReg;
	uint32_t clkEnMask;
} PeriphInfo;

uint32_t STM32RCC::_sysFreq = 8000000;
uint32_t STM32RCC::_ahbFreq;
uint32_t STM32RCC::_apb1Freq;
uint32_t STM32RCC::_apb2Freq;

static const PeriphInfo periphTable[] = {
#ifdef STM32F1
	{GPIOA, &RCC->APB2ENR, RCC_APB2ENR_IOPAEN},
	{GPIOB, &RCC->APB2ENR, RCC_APB2ENR_IOPBEN},
	{GPIOC, &RCC->APB2ENR, RCC_APB2ENR_IOPCEN},
	{GPIOD, &RCC->APB2ENR, RCC_APB2ENR_IOPDEN},
	#ifdef GPIOE
		{GPIOE, &RCC->APB2ENR, RCC_APB2ENR_IOPEEN},
	#endif
	#ifdef GPIOF
		{GPIOF, &RCC->APB2ENR, RCC_APB2ENR_IOPFEN},
	#endif
	#ifdef GPIOG
		{GPIOG, &RCC->APB2ENR, RCC_APB2ENR_IOPGEN},
	#endif
#else
	{GPIOA, &RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN},
	{GPIOB, &RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN},
	{GPIOC, &RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN},
	{GPIOD, &RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN},
	#ifdef GPIOE
		{GPIOE, &RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN},
	#endif
	#ifdef GPIOF
		{GPIOF, &RCC->AHB1ENR, RCC_AHB1ENR_GPIOFEN},
	#endif
	#ifdef GPIOG
		{GPIOG, &RCC->AHB1ENR, RCC_AHB1ENR_GPIOGEN},
	#endif
	#ifdef GPIOH
		{GPIOH, &RCC->AHB1ENR, RCC_AHB1ENR_GPIOHEN},
	#endif
	#ifdef GPIOI
		{GPIOI, &RCC->AHB1ENR, RCC_AHB1ENR_GPIOIEN},
	#endif
#endif
	{USART1, &RCC->APB2ENR, RCC_APB2ENR_USART1EN},
	{USART2, &RCC->APB1ENR, RCC_APB1ENR_USART2EN},
	{USART3, &RCC->APB1ENR, RCC_APB1ENR_USART3EN},
	{UART4, &RCC->APB1ENR, RCC_APB1ENR_UART4EN},
	{UART5, &RCC->APB1ENR, RCC_APB1ENR_UART5EN},
#ifdef USART6
	{USART6, &RCC->APB2ENR, RCC_APB2ENR_USART6EN},
#endif
	{I2C1, &RCC->APB1ENR, RCC_APB1ENR_I2C1EN},
	{I2C2, &RCC->APB1ENR, RCC_APB1ENR_I2C2EN},
#ifdef I2C3
	{I2C3, &RCC->APB1ENR, RCC_APB1ENR_I2C3EN},
#endif
#ifdef AFIO_BASE
	{AFIO, &RCC->APB2ENR, RCC_APB2ENR_AFIOEN},
#endif
#ifdef SYSCFG_BASE
	{SYSCFG, &RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN},
#endif
#ifdef STM32F1
	{USB, &RCC->APB1ENR, RCC_APB1ENR_USBEN},
#endif
	{TIM1, &RCC->APB2ENR, RCC_APB2ENR_TIM1EN},
	{TIM8, &RCC->APB2ENR, RCC_APB2ENR_TIM8EN},
	{TIM9, &RCC->APB2ENR, RCC_APB2ENR_TIM9EN},
	{TIM10, &RCC->APB2ENR, RCC_APB2ENR_TIM10EN},
	{TIM11, &RCC->APB2ENR, RCC_APB2ENR_TIM11EN},
	{TIM2, &RCC->APB1ENR, RCC_APB1ENR_TIM2EN},
	{TIM3, &RCC->APB1ENR, RCC_APB1ENR_TIM3EN},
	{TIM4, &RCC->APB1ENR, RCC_APB1ENR_TIM4EN},
	{TIM5, &RCC->APB1ENR, RCC_APB1ENR_TIM5EN},
	{TIM6, &RCC->APB1ENR, RCC_APB1ENR_TIM6EN},
	{TIM7, &RCC->APB1ENR, RCC_APB1ENR_TIM7EN},
	{TIM12, &RCC->APB1ENR, RCC_APB1ENR_TIM12EN},
	{TIM13, &RCC->APB1ENR, RCC_APB1ENR_TIM13EN},
	{TIM14, &RCC->APB1ENR, RCC_APB1ENR_TIM14EN},
	{}
};

STM32RCC::STM32RCC(STM32ClockSource src, uint32_t inClk, STM32ClockSource pllSrc, uint32_t outClk) {
	switch (src) {
		case STM32_CLOCKSOURCE_PLL:
			usePLL(pllSrc, inClk, outClk);
			break;
		case STM32_CLOCKSOURCE_HSE:
			useHSE(inClk);
			break;
		case STM32_CLOCKSOURCE_HSI:
		default:
			useHSI();
	}
}

void STM32RCC::enableClockSource(STM32ClockSource src) {
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

void STM32RCC::disableClockSource(STM32ClockSource src) {
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

bool STM32RCC::waitClockSourceReady(STM32ClockSource src, uint32_t timeout) {
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

void STM32RCC::selectClockSource(STM32ClockSource src) {
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

void STM32RCC::waitClockSourceSelected(STM32ClockSource src) {
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

#ifdef STM32F1
bool STM32RCC::setupPLL(STM32ClockSource src, uint32_t inClk, uint32_t outClk) {
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
			return true;
		}
	}
	return false;
}
#else
bool STM32RCC::setupPLL(STM32ClockSource src, uint32_t pllm, uint32_t plln, uint32_t pllp, uint32_t pllq) {
	if ((pllm >= 1) && (pllm <= 31) && (plln >= 1) && (plln <= 511) && (pllp >= 2) && (pllp <= 8) && (pllq >= 1) && (pllq <= 31)) {
		uint32_t cfg = (src == STM32_CLOCKSOURCE_HSE) ? RCC_PLLCFGR_PLLSRC : 0;
		cfg |= pllm << 0;
		cfg |= plln << 6;
		cfg |= ((pllp >> 1) - 1) << 16;
		cfg |= pllq << 24;
		RCC->PLLCFGR = cfg;
		return true;
	}
	return false;
}
		
bool STM32RCC::setupPLL(STM32ClockSource src, uint32_t inClk, uint32_t outClk) {
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
		return setupPLL(src, pllm, plln, pllp, pllq);
	}
	return false;
}
#endif

bool STM32RCC::useHSI() {
	enableClockSource(STM32_CLOCKSOURCE_HSI);
	waitClockSourceReady(STM32_CLOCKSOURCE_HSI);
	selectClockSource(STM32_CLOCKSOURCE_HSI);
	waitClockSourceSelected(STM32_CLOCKSOURCE_HSI);
	FLASH_ACR = FLASH_ACR & ~FLASH_ACR_LATENCY_MASK;
	setAHBPrescaler(RCC_AHB_NODIV);
	setAPB1Prescaler(RCC_APB_NODIV);
	setAPB2Prescaler(RCC_APB_NODIV);
	_sysFreq = 8000000;
	_ahbFreq = _sysFreq;
	_apb1Freq = _sysFreq;
	_apb2Freq = _sysFreq;
	return true;
}

bool STM32RCC::useHSE(uint32_t clk) {
	if (clk) {
		enableClockSource(STM32_CLOCKSOURCE_HSE);
		if (waitClockSourceReady(STM32_CLOCKSOURCE_HSE)) {
			setAHBPrescaler(RCC_AHB_NODIV);
			setAPB1Prescaler(RCC_APB_NODIV);
			setAPB2Prescaler(RCC_APB_NODIV);
			selectClockSource(STM32_CLOCKSOURCE_HSE);
			waitClockSourceSelected(STM32_CLOCKSOURCE_HSE);
			FLASH_ACR = (FLASH_ACR & ~FLASH_ACR_LATENCY_MASK) | ((clk - 1) / 24000000);
			_sysFreq = clk;
			_ahbFreq = _sysFreq;
			_apb1Freq = _sysFreq;
			_apb2Freq = _sysFreq;
			return true;
		}
	}
	return false;
}

bool STM32RCC::usePLL(STM32ClockSource src, uint32_t inClk, uint32_t outClk) {
	useHSI();
	enableClockSource(src);
	if (waitClockSourceReady(src)) {
		if (setupPLL(src, inClk, outClk)) {
			enableClockSource(STM32_CLOCKSOURCE_PLL);
			if (waitClockSourceReady(STM32_CLOCKSOURCE_PLL)) {
				_sysFreq = outClk;
				setAHBPrescaler(RCC_AHB_NODIV);
				_ahbFreq = _sysFreq;
#ifdef STM32F1
				setADCPrescaler(3);
				setAPB1Prescaler(RCC_APB_DIV2);
				setAPB2Prescaler(RCC_APB_NODIV);
				_apb1Freq = _sysFreq / 2;
				_apb2Freq = _sysFreq;
#else
				setAPB1Prescaler(RCC_APB_DIV4);
				setAPB2Prescaler(RCC_APB_DIV2);
				_apb1Freq = _sysFreq / 4;
				_apb2Freq = _sysFreq / 2;
#endif
#ifdef STM32F1
				FLASH_ACR = (FLASH_ACR & ~FLASH_ACR_LATENCY_MASK) | FLASH_ACR_PRFTBE | ((outClk - 1) / 24000000);
#else
				FLASH_ACR = (FLASH_ACR & ~FLASH_ACR_LATENCY_MASK) | ((outClk - 1) / 24000000);
#endif
				selectClockSource(STM32_CLOCKSOURCE_PLL);
				waitClockSourceSelected(STM32_CLOCKSOURCE_PLL);
				disableClockSource(STM32_CLOCKSOURCE_HSI);
				return true;
			} else {
				disableClockSource(STM32_CLOCKSOURCE_PLL);
			}
		}
	}
	disableClockSource(STM32_CLOCKSOURCE_HSE);
	return false;
}

static const PeriphInfo *rccSearchPeriph(volatile void *periph) {
	const PeriphInfo *info = periphTable;
	while (info->base && (info->base != periph)) {
		info++;
	}
	return info->base ? info : 0;
}

void STM32RCC::periphClockEnable(volatile void *periph) {
	const PeriphInfo *info = rccSearchPeriph(periph);
	if (info) {
		*info->clkEnReg |= info->clkEnMask;
	}
}

void STM32RCC::periphClockDisable(volatile void *periph) {
	const PeriphInfo *info = rccSearchPeriph(periph);
	if (info) {
		*info->clkEnReg &= ~info->clkEnMask;
	}
}
