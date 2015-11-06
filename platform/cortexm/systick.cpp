#include <cortexm.h>

uint32_t SysTick::frequency;
volatile uint64_t SysTick::counter;

SysTick::SysTick(uint32_t freq, uint32_t sysFreq) {
	if (freq != 0) {
		setFrequency(freq, sysFreq);
		enableCounter();
		enableInterrupt();
	}
}

bool SysTick::setFrequency(uint32_t freq, uint32_t coreFreq) {
	uint32_t clksrc;
	uint32_t div = coreFreq / freq;
#if defined(__ARM_ARCH_6M__)
	clksrc = SYSTICK_CTRL_CLKSOURCE_AHB;
#else
	if (div >= SYSTICK_LOAD_MASK) {
		div /= 8;
		clksrc = SYSTICK_CTRL_CLKSOURCE_AHB_DIV8;
	} else {
		clksrc = SYSTICK_CTRL_CLKSOURCE_AHB;
	}
#endif
	if (div >= SYSTICK_LOAD_MASK) {
		return false;
	}
	setClockSource(clksrc);
	setReloadValue(div - 1);
	frequency = freq;
	return true;
}

void SysTick::interruptHandler() {
	counter++;
}

uint64_t SysTick::timestamp() {
	return counter * 1000000 / frequency + (SYSTICK->LOAD - SYSTICK->VAL) / ((SYSTICK->LOAD + 1) / frequency);
}

void SysTick::sleep(uint64_t microseconds) {
	uint64_t target = timestamp() + microseconds;
	while (timestamp() < target);
}
