#include <platform.h>

static uint32_t sysTickFrequency;
static volatile uint64_t sysTickCounter;

ISR(SYSTICK_vect) {
	sysTickCounter++;
}

void sysTickSetClockSource(uint32_t src) {
	SYSTICK->CTRL = (SYSTICK->CTRL & ~SYSTICK_CTRL_CLKSOURCE) | src;
}

void sysTickSetReloadValue(uint32_t value) {
	SYSTICK->LOAD = value & SYSTICK_LOAD_MASK;
}

uint32_t sysTickGetReloadValue(void) {
	return SYSTICK->LOAD;
}

void sysTickEnableCounter(void) {
	SYSTICK->CTRL |= SYSTICK_CTRL_ENABLE;
}

void sysTickDisableCounter(void) {
	SYSTICK->CTRL &= ~SYSTICK_CTRL_ENABLE;
}

void sysTickEnableInterrupt(void) {
	SYSTICK->CTRL |= SYSTICK_CTRL_TICKINT;
}

void sysTickDisableInterrupt(void) {
	SYSTICK->CTRL &= ~SYSTICK_CTRL_TICKINT;
}

int sysTickSetup(uint32_t freq) {
	sysTickDisableInterrupt();
	sysTickDisableCounter();
	uint32_t clksrc;
	uint32_t div = AHB_FREQUENCY / freq;
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
		return 0;
	}
	sysTickSetClockSource(clksrc);
	sysTickSetReloadValue(div - 1);
	sysTickFrequency = freq;
	sysTickEnableInterrupt();
	sysTickEnableCounter();
	return 1;
}

uint32_t sysTickGetFrequency(void) {
	return sysTickFrequency;
}

uint64_t sysTickTimestamp(void) {
	return sysTickCounter * 1000000 / sysTickFrequency + (SYSTICK->LOAD - SYSTICK->VAL) / ((SYSTICK->LOAD + 1) / sysTickFrequency);
}

void sysTickDelay(uint64_t time) {
	uint64_t targetTime = sysTickTimestamp() + time;
	while (sysTickTimestamp() < targetTime) {
		// Busy loop
	}
}

void usleep(uint64_t time) {
	sysTickDelay(time);
}