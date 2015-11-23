/**
	\addtogroup STM32 STM32
	@{
*/
#ifndef __STM32_H__
#define __STM32_H__

#include <stdint.h>
#include <stm32defs.h>
#include <cortexm.h>
#include <stm32_rcc.h>
#include <stm32_gpio.h>
#include <stm32_timer.h>
#include <stm32_usart.h>
#include <stm32_i2c.h>
#include <stm32_usb.h>
#include <stm32_adc.h>

inline void platform_init() {
#if defined STM32F4 || defined STM32F3
	SCB->CPACR |= SCB_CPACR_FULL * (SCB_CPACR_CP10 | SCB_CPACR_CP11);
#endif
}

#endif
/** @} */
