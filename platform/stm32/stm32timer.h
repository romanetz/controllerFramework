#ifndef __STM32TIMER_H__
#define __STM32TIMER_H__

#include <generic.h>
#include <timer.h>
#include <stm32timerregs.h>

void stm32_timerEnable(volatile STM32TimerRegs *timer);
void stm32_timerDisable(volatile STM32TimerRegs *timer);

void stm32_timerSetPrescaler(volatile STM32TimerRegs *timer, uint16_t prescaler);
void stm32_timerSetReloadValue(volatile STM32TimerRegs *timer, uint16_t value);
void stm32_timerSetFrequency(volatile STM32TimerRegs *timer, uint32_t frequency);

void stm32_timerStart(volatile STM32TimerRegs *timer);
void stm32_timerStop(volatile STM32TimerRegs *timer);

#ifdef USE_CLASSES

typedef struct STM32TimerClass STM32TimerClass;

struct STM32TimerClass {
	TimerClass parent;
	volatile STM32TimerRegs *regs;
	TimerCallback callback;
	void *arg;
};

#define STM32_TIMER_CLASS(obj) ((STM32TimerClass*)(obj))

STM32TimerClass *stm32_timerClassInit(STM32TimerClass *timer, volatile STM32TimerRegs *regs);

#endif

#endif
