#ifndef __STM32GPIO_H__
#define __STM32GPIO_H__

#include <stm32.h>
#include <stm32gpioregs.h>
#include <gpio.h>

#define STM32_GPIO0 (1 << 0)
#define STM32_GPIO1 (1 << 1)
#define STM32_GPIO2 (1 << 2)
#define STM32_GPIO3 (1 << 3)
#define STM32_GPIO4 (1 << 4)
#define STM32_GPIO5 (1 << 5)
#define STM32_GPIO6 (1 << 6)
#define STM32_GPIO7 (1 << 7)
#define STM32_GPIO8 (1 << 8)
#define STM32_GPIO9 (1 << 9)
#define STM32_GPIO10 (1 << 10)
#define STM32_GPIO11 (1 << 11)
#define STM32_GPIO12 (1 << 12)
#define STM32_GPIO13 (1 << 13)
#define STM32_GPIO14 (1 << 14)
#define STM32_GPIO15 (1 << 15)

void stm32_gpioPortEnable(volatile STM32GPIORegs *port);
void stm32_gpioPortDisable(volatile STM32GPIORegs *port);

void stm32_gpioPortSetup(volatile STM32GPIORegs *port, uint16_t mask, GPIOMode mode, GPIODriverType driver, GPIOPuPd pupd);
#ifndef STM32F1
void stm32_gpioPortSetupAltFn(volatile STM32GPIORegs *port, uint16_t mask, GPIODriverType driver, GPIOPuPd pupd, uint8_t fn);
#else
void stm32_gpioPortSetupAltFn(volatile STM32GPIORegs *port, uint16_t mask, GPIODriverType driver, GPIOPuPd pupd);
#endif
#define stm32_gpioPortWrite(port, value) (port)->ODR = value;
#define stm32_gpioPortRead(port) (port)->IDR
#define stm32_gpioPortReadBits(port, mask) ((port)->IDR & (mask))
#define stm32_gpioPortSetBits(port, mask) (port)->ODR |= mask
#define stm32_gpioPortClearBits(port, mask) (port)->ODR &= ~(mask)
#define stm32_gpioPortToggleBits(port, mask) (port)->ODR ^= mask
BOOL stm32_gpioPortAttachInterrupt(volatile STM32GPIORegs *port, uint8_t bit, GPIOInterruptMode mode, GPIOInterruptHandler handler, void *arg);

#ifdef USE_CLASSES

typedef struct STM32GPIOClass {
	GPIOClass parent;
	volatile STM32GPIORegs *port;
	uint16_t mask;
} STM32GPIOClass;

#define STM32_GPIO_CLASS(obj) ((STM32GPIOClass*)(obj))

STM32GPIOClass *stm32_gpioClassInit(STM32GPIOClass *obj, volatile STM32GPIORegs *port, uint16_t mask);

#endif

#endif
