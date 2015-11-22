#ifndef __STM32_TIMER_H__
#define __STM32_TIMER_H__

#include <stm32.h>
#include <timer.h>

struct STM32TimerRegs {
	uint32_t CR1; // 0x00
	uint32_t CR2; // 0x04
	uint32_t SMCR; // 0x08
	uint32_t DIER; // 0x0C
	uint32_t SR; // 0x10
	uint32_t EGR; // 0x14
	uint32_t CCMR1; // 0x18
	uint32_t CCMR2; // 0x1C
	uint32_t CCER; // 0x20
	uint32_t CNT; // 0x24
	uint32_t PSC; // 0x28
	uint32_t ARR; // 0x2C
	uint32_t RCR; // 0x30
	uint32_t CCR1; // 0x34
	uint32_t CCR2; // 0x38
	uint32_t CCR3; // 0x3C
	uint32_t CCR4; // 0x40
	uint32_t BDTR; // 0x44
	uint32_t DCR; // 0x48
	uint32_t DMAR; // 0x4C
};

#define TIM1 MMIO(TIM1_BASE, STM32TimerRegs)
#define TIM2 MMIO(TIM2_BASE, STM32TimerRegs)
#define TIM3 MMIO(TIM3_BASE, STM32TimerRegs)
#define TIM4 MMIO(TIM4_BASE, STM32TimerRegs)
#define TIM5 MMIO(TIM5_BASE, STM32TimerRegs)
#define TIM6 MMIO(TIM6_BASE, STM32TimerRegs)
#define TIM7 MMIO(TIM7_BASE, STM32TimerRegs)
#define TIM8 MMIO(TIM8_BASE, STM32TimerRegs)
#define TIM9 MMIO(TIM9_BASE, STM32TimerRegs)
#define TIM10 MMIO(TIM10_BASE, STM32TimerRegs)
#define TIM11 MMIO(TIM11_BASE, STM32TimerRegs)
#define TIM12 MMIO(TIM12_BASE, STM32TimerRegs)
#define TIM13 MMIO(TIM13_BASE, STM32TimerRegs)
#define TIM14 MMIO(TIM14_BASE, STM32TimerRegs)

class STM32Timer: public Timer {
	private:
		volatile STM32TimerRegs *_timer;
		
		int _frequency;
		
		TimerCallback _callback;
		
		void *_callbackArg;
		
	public:
		STM32Timer(volatile STM32TimerRegs *timer);
		
		STM32Timer(volatile STM32TimerRegs *timer, int freq, TimerCallback callback = 0, void *arg = 0, bool enable = false);
		
		~STM32Timer();
		
		int frequency() { return _frequency; }
		
		void setFrequency(int freq);
		
		void setFrequency(uint16_t prescaler, uint16_t reloadValue);
		
		void setCallback(TimerCallback callback, void *arg = 0);
		
		void enable();
		
		void disable();
		
		bool enabled();
		
		uint32_t counter() { return _timer->CNT; };
		
		void setCounter(uint32_t value) { _timer->CNT = value; };
		
		uint16_t prescaler() { return _timer->PSC; };
		
		uint16_t reloadValue() { return _timer->ARR; }
		
		volatile STM32TimerRegs *mmioRegs() { return _timer; };
		
		void interruptHandler();
		
};

#endif
