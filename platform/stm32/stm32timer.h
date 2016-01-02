#ifndef __STM32TIMER_H__
#define __STM32TIMER_H__

#include <generic.h>
#include <timer.h>
#include <pwm.h>
#include <stm32timerregs.h>

void stm32_timerEnable(volatile STM32TimerRegs *timer);
void stm32_timerDisable(volatile STM32TimerRegs *timer);

void stm32_timerSetCounter(volatile STM32TimerRegs *timer, uint16_t value);
uint16_t stm32_timerGetCounter(volatile STM32TimerRegs *timer);

void stm32_timerSetPrescaler(volatile STM32TimerRegs *timer, uint16_t prescaler);
void stm32_timerSetReloadValue(volatile STM32TimerRegs *timer, uint16_t value);
uint16_t stm32_timerGetPrescaler(volatile STM32TimerRegs *timer);
uint16_t stm32_timerGetReloadValue(volatile STM32TimerRegs *timer);
void stm32_timerSetFrequency(volatile STM32TimerRegs *timer, uint32_t frequency);

void stm32_timerStart(volatile STM32TimerRegs *timer);
void stm32_timerStop(volatile STM32TimerRegs *timer);

void stm32_timerSetOutputCompareMode(volatile STM32TimerRegs *timer, uint8_t channel, uint8_t mode);
void stm32_timerEnableOutputCompare(volatile STM32TimerRegs *timer, uint8_t channel);
void stm32_timerDisableOutputCompare(volatile STM32TimerRegs *timer, uint8_t channel);
void stm32_timerSetOutputCompareValue(volatile STM32TimerRegs *timer, uint8_t channel, uint16_t value);

#ifdef USE_CLASSES

typedef struct STM32TimerClass STM32TimerClass;

typedef struct STM32PWMClass STM32PWMClass;

struct STM32TimerClass {
	TimerClass parent;
	volatile STM32TimerRegs *regs;
	TimerCallback callback;
	void *arg;
};

struct STM32PWMClass {
	PWMClass parent;
	volatile STM32TimerRegs *regs;
};

#define STM32_TIMER_CLASS(obj) ((STM32TimerClass*)(obj))

#define STM32_PWM_CLASS(obj) ((STM32PWMClass*)(obj))

STM32TimerClass *stm32_timerClassInit(STM32TimerClass *timer, volatile STM32TimerRegs *regs);

STM32PWMClass *stm32_pwmClassInit(STM32PWMClass *pwm, volatile STM32TimerRegs *regs);

#endif

#endif
