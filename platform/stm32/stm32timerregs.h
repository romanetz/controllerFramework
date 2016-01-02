#ifndef __STM32TIMERREGS_H__
#define __STM32TIMERREGS_H__

#include <stdint.h>

typedef struct STM32TimerRegs {
	uint32_t CR1; // 0x00
	uint32_t CR2; // 0x04
	uint32_t SMCR; // 0x08
	uint32_t DIER; // 0x0C
	uint32_t SR; // 0x10
	uint32_t EGR; // 0x14
	uint32_t CCMR[2]; // 0x18 - 0x1C
	uint32_t CCER; // 0x20
	uint32_t CNT; // 0x24
	uint32_t PSC; // 0x28
	uint32_t ARR; // 0x2C
	uint32_t RCR; // 0x30
	uint32_t CCR[4]; // 0x34 - 0x40
	uint32_t BDTR; // 0x44
	uint32_t DCR; // 0x48
	uint32_t DMAR; // 0x4C
} STM32TimerRegs;

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

#endif
