#ifndef __STM32_H__
#define __STM32_H__

#include <arm-cm.h>
#include <stm32defs.h>
#include <stm32rcc.h>
#include <stm32periphinfo.h>
#include <stm32gpio.h>
#include <stm32usart.h>
#include <stm32i2c.h>
#include <stm32usb.h>
#include <stm32usbotg.h>

#if defined STM32F4 || defined STM32F3
#define ARM_PLATFORM_INIT do { SCB->CPACR |= SCB_CPACR_FULL * (SCB_CPACR_CP10 | SCB_CPACR_CP11); } while (0)
#endif

#define AHB_FREQUENCY stm32_rccAHBFrequency()

#endif
