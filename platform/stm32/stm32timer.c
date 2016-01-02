#include <math.h>
#include <stm32.h>

void stm32_timerEnable(volatile STM32TimerRegs *timer) {
	stm32_rccPeriphClockEnable(timer);
}

void stm32_timerDisable(volatile STM32TimerRegs *timer) {
	stm32_rccPeriphClockDisable(timer);
}

void stm32_timerSetPrescaler(volatile STM32TimerRegs *timer, uint16_t prescaler) {
	timer->PSC = prescaler;
}

void stm32_timerSetReloadValue(volatile STM32TimerRegs *timer, uint16_t value) {
	timer->ARR = value;
}

void stm32_timerSetFrequency(volatile STM32TimerRegs *timer, uint32_t freq) {
	uint32_t busFreq;
	switch ((uint32_t)timer) {
		case TIM1_BASE:
		case TIM8_BASE:
		case TIM9_BASE:
		case TIM10_BASE:
		case TIM11_BASE:
			busFreq = stm32_rccAPB2Frequency();
			break;
		default:
			busFreq = stm32_rccAPB1Frequency();
	}
	uint32_t div = busFreq / freq;
	uint32_t prescaler = 1;
	if (div >= 65536) {
		uint32_t maxPrescaler = sqrtf(div);
		if (maxPrescaler < 32768.0f) {
			for (prescaler = 2; prescaler < maxPrescaler; prescaler++) {
				div = (uint16_t)(busFreq / prescaler / freq);
				if ((busFreq / (prescaler * div)) <= (uint32_t)freq) break;
			}
		} else {
			for (div = maxPrescaler; div < 65536; div++) {
				prescaler = (uint16_t)(busFreq / div / freq);
				if ((busFreq / (prescaler * div)) <= (uint32_t)freq) break;
			}
		}
	}
	stm32_timerSetPrescaler(timer, prescaler - 1);
	stm32_timerSetReloadValue(timer, div);
}

void stm32_timerStart(volatile STM32TimerRegs *timer) {
	timer->CR1 |= TIM1_CR1_CEN | TIM1_CR1_ARPE;
}

void stm32_timerStop(volatile STM32TimerRegs *timer) {
	timer->CR1 &= ~TIM1_CR1_CEN;
}

#ifdef USE_CLASSES

static STM32TimerClass *tim1;
static STM32TimerClass *tim2;
static STM32TimerClass *tim3;
static STM32TimerClass *tim4;
static STM32TimerClass *tim5;
static STM32TimerClass *tim6;
static STM32TimerClass *tim7;
static STM32TimerClass *tim8;
static STM32TimerClass *tim9;
static STM32TimerClass *tim10;
static STM32TimerClass *tim11;
static STM32TimerClass *tim12;
static STM32TimerClass *tim13;
static STM32TimerClass *tim14;

static STM32TimerClass **timerObj(volatile STM32TimerRegs *timer) {
	switch ((uint32_t)timer) {
		case TIM1_BASE:
			return &tim1;
		case TIM2_BASE:
			return &tim2;
		case TIM3_BASE:
			return &tim3;
		case TIM4_BASE:
			return &tim4;
		case TIM5_BASE:
			return &tim5;
		case TIM6_BASE:
			return &tim6;
		case TIM7_BASE:
			return &tim7;
		case TIM8_BASE:
			return &tim8;
		case TIM9_BASE:
			return &tim9;
		case TIM10_BASE:
			return &tim10;
		case TIM11_BASE:
			return &tim11;
		case TIM12_BASE:
			return &tim12;
		case TIM13_BASE:
			return &tim13;
		case TIM14_BASE:
			return &tim14;
		default:
			return NULL;
	}
}

static uint8_t timerIRQ(volatile STM32TimerRegs *timer) {
	switch ((uint32_t)timer) {
		case TIM1_BASE:
		case TIM10_BASE:
			return TIM1_UP_TIM10_IRQ;
		case TIM2_BASE:
			return TIM2_IRQ;
		case TIM3_BASE:
			return TIM3_IRQ;
		case TIM4_BASE:
			return TIM4_IRQ;
		case TIM5_BASE:
			return TIM5_IRQ;
#ifdef TIM6_IRQ
		case TIM6_BASE:
			return TIM6_IRQ;
#endif
		case TIM7_BASE:
			return TIM7_IRQ;
		case TIM8_BASE:
		case TIM13_BASE:
			return TIM8_UP_TIM13_IRQ;
		case TIM9_BASE:
			return TIM1_BRK_TIM9_IRQ;
		case TIM11_BASE:
			return TIM1_TRG_COM_TIM11_IRQ;
		case TIM12_BASE:
			return TIM8_BRK_TIM12_IRQ;
		case TIM14_BASE:
			return TIM8_TRG_COM_TIM14_IRQ;
		default:
			return -1;
	}
}

static void STM32TimerClass_setFrequency(TimerClass *_timer, uint32_t frequency) {
	STM32TimerClass *timer = STM32_TIMER_CLASS(_timer);
	stm32_timerSetFrequency(timer->regs, frequency);
}

static void STM32TimerClass_setCallback(TimerClass *_timer, TimerCallback callback, void *arg) {
	STM32TimerClass *timer = STM32_TIMER_CLASS(_timer);
	timer->regs->DIER &= ~TIM1_DIER_UIE;
	timer->callback = callback;
	timer->arg = arg;
	if (timer->callback) {
		timer->regs->DIER |= TIM1_DIER_UIE;
	}
}

static void STM32TimerClass_start(TimerClass *_timer) {
	STM32TimerClass *timer = STM32_TIMER_CLASS(_timer);
	stm32_timerStart(timer->regs);
}

static void STM32TimerClass_stop(TimerClass *_timer) {
	STM32TimerClass *timer = STM32_TIMER_CLASS(_timer);
	stm32_timerStop(timer->regs);
}

STM32TimerClass *stm32_timerClassInit(STM32TimerClass *timer, volatile STM32TimerRegs *regs) {
	TIMER_CLASS(timer)->setFrequency = STM32TimerClass_setFrequency;
	TIMER_CLASS(timer)->setCallback = STM32TimerClass_setCallback;
	TIMER_CLASS(timer)->start = STM32TimerClass_start;
	TIMER_CLASS(timer)->stop = STM32TimerClass_stop;
	timer->regs = regs;
	stm32_timerEnable(regs);
	STM32TimerClass **obj = timerObj(regs);
	if (obj) *obj = timer;
	nvicEnableIRQ(timerIRQ(regs));
	return timer;
}

void STM32TimerClass_interruptHandler(STM32TimerClass *timer) {
	if (timer->regs->SR & TIM1_SR_UIF) {
		timer->regs->SR = ~TIM1_SR_UIF;
		if (timer->callback) {
			timer->callback(TIMER_CLASS(timer), timer->arg);
		}
	}
}

ISR(TIM1_UP_TIM10_vect) {
	if (tim1) STM32TimerClass_interruptHandler(tim1);
	if (tim10) STM32TimerClass_interruptHandler(tim10);
}

ISR(TIM2_vect) {
	if (tim2) STM32TimerClass_interruptHandler(tim2);
}

ISR(TIM3_vect) {
	if (tim3) STM32TimerClass_interruptHandler(tim3);
}

ISR(TIM4_vect) {
	if (tim4) STM32TimerClass_interruptHandler(tim4);
}

ISR(TIM5_vect) {
	if (tim5) STM32TimerClass_interruptHandler(tim5);
}

ISR(TIM6_vect) {
	if (tim6) STM32TimerClass_interruptHandler(tim6);
}

ISR(TIM7_vect) {
	if (tim7) STM32TimerClass_interruptHandler(tim7);
}

ISR(TIM8_UP_TIM13_vect) {
	if (tim8) STM32TimerClass_interruptHandler(tim8);
	if (tim13) STM32TimerClass_interruptHandler(tim13);
}

ISR(TIM1_BRK_TIM9_vect) {
	if (tim1) STM32TimerClass_interruptHandler(tim1);
	if (tim9) STM32TimerClass_interruptHandler(tim9);
}

ISR(TIM1_TRG_COM_TIM11_vect) {
	if (tim1) STM32TimerClass_interruptHandler(tim1);
	if (tim11) STM32TimerClass_interruptHandler(tim11);
}

ISR(TIM8_BRK_TIM12_vect) {
	if (tim8) STM32TimerClass_interruptHandler(tim8);
	if (tim12) STM32TimerClass_interruptHandler(tim12);
}

ISR(TIM8_TRG_COM_TIM14_vect) {
	if (tim8) STM32TimerClass_interruptHandler(tim8);
	if (tim14) STM32TimerClass_interruptHandler(tim14);
}

#endif