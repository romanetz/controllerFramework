#include <platform.h>

static uint32_t sysTickFrequency;
volatile timestamp_t sysTickCounter;

ISR(SYSTICK_vect) {
	sysTickCounter++;
	executeTaskSheduler();
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

timestamp_t sysTickTimestamp(void) {
	return sysTickCounter * 1000000 / sysTickFrequency + (SYSTICK->LOAD - SYSTICK->VAL) / ((SYSTICK->LOAD + 1) / sysTickFrequency);
}

void sysTickDelay(timestamp_t time) {
	timestamp_t targetTime = sysTickTimestamp() + time;
	while (sysTickTimestamp() < targetTime) {
		// Busy loop
	}
}

void usleep(timestamp_t time) {
	timestamp_t tickPeriod = 1000000 / sysTickFrequency;
	if (time < tickPeriod) {
		sysTickDelay(time);
	} else {
		if (!threadSleepUntil(sysTickCounter + time / tickPeriod)) {
			sysTickDelay(time);
		}
	}
}