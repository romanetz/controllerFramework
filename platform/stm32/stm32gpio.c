#include <stm32.h>

static struct {
	GPIOInterruptHandler func;
	void *arg;
} gpioInterruptHandlers[16];

void stm32_gpioPortEnable(volatile STM32GPIORegs *port) {
	stm32_rccPeriphClockEnable(port);
}

void stm32_gpioPortDisable(volatile STM32GPIORegs *port) {
	stm32_rccPeriphClockDisable(port);
}

void stm32_gpioPortSetup(volatile STM32GPIORegs *port, uint16_t mask, GPIOMode mode, GPIODriverType driver, GPIOPuPd pupd) {
#ifdef STM32F1
	uint32_t crValue = 0, i;
	switch (mode) {
		case GPIO_MODE_INPUT:
			crValue = STM32_GPIO_MODE_INPUT;
			if (pupd == GPIO_PUPD_NONE) {
				crValue |= STM32_GPIO_CNF_INPUT_FLOAT;
			} else {
				crValue |= STM32_GPIO_CNF_INPUT_PULL_UPDOWN;
			}
			break;
		case GPIO_MODE_ANALOG:
			crValue = STM32_GPIO_MODE_INPUT | STM32_GPIO_CNF_INPUT_ANALOG;
			break;
		case GPIO_MODE_OUTPUT:
			crValue = STM32_GPIO_MODE_OUTPUT_50_MHZ;
			if (driver == GPIO_DRIVER_OPENDRAIN) {
				crValue |= STM32_GPIO_CNF_OUTPUT_OPENDRAIN;
			} else {
				crValue |= STM32_GPIO_CNF_OUTPUT_PUSHPULL;
			}
			break;
		case GPIO_MODE_ALTFN:
			crValue = STM32_GPIO_MODE_OUTPUT_50_MHZ;
			if (driver == GPIO_DRIVER_OPENDRAIN) {
				crValue |= STM32_GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN;
			} else {
				crValue |= STM32_GPIO_CNF_OUTPUT_ALTFN_PUSHPULL;
			}
			break;
	}
	for (i = 0; i < 16; i++) {
		if (mask & (1 << i)) {
			volatile uint32_t *reg = (i < 8) ? &port->CRL : &port->CRH;
			uint32_t offset = (i % 8) * 4;
			*reg &= ~(0x0F << offset);
			*reg |= crValue << offset;
		}
	}
	if ((mode == GPIO_MODE_INPUT) && (pupd != GPIO_PUPD_NONE)) {
		if (pupd == GPIO_PUPD_PULLDOWN) {
			stm32_gpioPortClearBits(port, mask);
		} else {
			stm32_gpioPortSetBits(port, mask);
		}
	}
#else
	uint32_t moder = 0, otyper = 0, ospeedr = 0, pupdr = 0, i;
	switch (mode) {
		case GPIO_MODE_INPUT:
			moder = STM32_GPIO_MODE_INPUT;
			break;
		case GPIO_MODE_ANALOG:
			moder = STM32_GPIO_MODE_ANALOG;
			break;
		case GPIO_MODE_OUTPUT:
			moder = STM32_GPIO_MODE_OUTPUT;
			break;
		case GPIO_MODE_ALTFN:
			moder = STM32_GPIO_MODE_AF;
			break;
	}
	if (driver == GPIO_DRIVER_OPENDRAIN) {
		otyper = STM32_GPIO_OTYPE_OD;
	} else {
		otyper = STM32_GPIO_OTYPE_PP;
	}
	ospeedr = STM32_GPIO_SPEED_MAX;
	switch (pupd) {
		case GPIO_PUPD_PULLUP:
			pupdr = STM32_GPIO_PUPD_PULLUP;
			break;
		case GPIO_PUPD_PULLDOWN:
			pupdr = STM32_GPIO_PUPD_PULLDOWN;
			break;
		default:
			pupdr = STM32_GPIO_PUPD_NONE;
	}
	for (i = 0; i < 16; i++) {
		if (mask & (1 << i)) {
			uint32_t offset = i * 2;
			port->MODER &= ~(3 << offset);
			port->MODER |= moder << offset;
			port->OTYPER &= ~(3 << offset);
			port->OTYPER |= otyper << offset;
			port->OSPEEDR &= ~(3 << offset);
			port->OSPEEDR |= ospeedr << offset;
			port->PUPDR &= ~(3 << offset);
			port->PUPDR |= pupdr << offset;
		}
	}
#endif
}

#ifndef STM32F1
void stm32_gpioPortSetupAltFn(volatile STM32GPIORegs *port, uint16_t mask, GPIODriverType driver, GPIOPuPd pupd, uint8_t fn) {
	uint32_t i;
	stm32_gpioPortSetup(port, mask, GPIO_MODE_ALTFN, driver, pupd);
	for (i = 0; i < 16; i++) {
		if (mask & (1 << i)) {
			volatile uint32_t *reg = (i < 8) ? &port->AFRL : &port->AFRH;
			uint32_t offset = (i % 8) * 4;
			*reg &= ~(0x0F << offset);
			*reg |= (uint32_t)fn << offset;
		}
	}
}
#else
void stm32_gpioPortSetupAltFn(volatile STM32GPIORegs *port, uint16_t mask, GPIODriverType driver, GPIOPuPd pupd) {
	stm32_gpioPortSetup(port, mask, GPIO_MODE_ALTFN, driver, pupd);
}
#endif

static int gpioPortIndex(volatile STM32GPIORegs *gpio) {
	switch ((uint32_t)gpio) {
		case GPIOA_BASE:
			return 0;
		case GPIOB_BASE:
			return 1;
		case GPIOC_BASE:
			return 2;
		case GPIOD_BASE:
			return 3;
#ifdef GPIOE
		case GPIOE_BASE:
			return 4;
#endif
#ifdef GPIOF
		case GPIOF_BASE:
			return 5;
#endif
#ifdef GPIOG
		case GPIOG_BASE:
			return 6;
#endif
#ifdef GPIOH
		case GPIOH_BASE:
			return 7;
#endif
#ifdef GPIOI
		case GPIOI_BASE:
			return 8;
#endif
		default:
			return -1;
	}
}

uint8_t stm32_gpioPortAttachInterrupt(volatile STM32GPIORegs *port, uint8_t bit, GPIOInterruptMode mode, GPIOInterruptHandler handler, void *arg) {
	int portIndex = gpioPortIndex(port);
	if ((portIndex >= 0) && (bit < 16)) {
		EXTI->IMR &= ~(1 << bit);
#ifdef AFIO_BASE
		stm32_rccPeriphClockEnable(AFIO);
		AFIO->EXTICR[bit / 4] &= ~(0xF << ((bit % 4) * 4));
		AFIO->EXTICR[bit / 4] |= portIndex << ((bit % 4) * 4);
#endif
#ifdef SYSCFG_BASE
		stm32_rccPeriphClockEnable(SYSCFG);
		SYSCFG->EXTICR[bit / 4] &= ~(0xF << ((bit % 4) * 4));
		SYSCFG->EXTICR[bit / 4] |= portIndex << ((bit % 4) * 4);
#endif
		gpioInterruptHandlers[bit].func = handler;
		gpioInterruptHandlers[bit].arg = arg;
		if (mode & GPIO_INTERRUPT_RISING) {
			EXTI->RTSR |= 1 << bit;
		} else {
			EXTI->RTSR &= ~(1 << bit);
		}
		if (mode & GPIO_INTERRUPT_FALLING) {
			EXTI->FTSR |= 1 << bit;
		} else {
			EXTI->FTSR &= ~(1 << bit);
		}
		EXTI->IMR |= 1 << bit;
		uint8_t irq;
		switch (bit) {
			case 0:
				irq = EXTI0_IRQ;
				break;
			case 1:
				irq = EXTI1_IRQ;
				break;
			case 2:
				irq = EXTI2_IRQ;
				break;
			case 3:
				irq = EXTI3_IRQ;
				break;
			case 4:
				irq = EXTI4_IRQ;
				break;
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
				irq = EXTI9_5_IRQ;
				break;
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				irq = EXTI15_10_IRQ;
				break;
		}
		nvicEnableIRQ(irq);
		return 1;
	}
	return 0;
}

static void STM32GPIOClass_setup(GPIOClass *gpio, GPIOMode mode, GPIODriverType driver, GPIOPuPd pupd) {
	STM32GPIOClass *stm32gpio = STM32_GPIO_CLASS(gpio);
	stm32_gpioPortSetup(stm32gpio->port, stm32gpio->mask, mode, driver, pupd);
}

static uint8_t STM32GPIOClass_read(GPIOClass *gpio) {
	STM32GPIOClass *stm32gpio = STM32_GPIO_CLASS(gpio);
	return stm32_gpioPortReadBits(stm32gpio->port, stm32gpio->mask) ? 1 : 0; 
}

static void STM32GPIOClass_set(GPIOClass *gpio) {
	STM32GPIOClass *stm32gpio = STM32_GPIO_CLASS(gpio);
	stm32_gpioPortSetBits(stm32gpio->port, stm32gpio->mask);
}

static void STM32GPIOClass_clear(GPIOClass *gpio) {
	STM32GPIOClass *stm32gpio = STM32_GPIO_CLASS(gpio);
	stm32_gpioPortClearBits(stm32gpio->port, stm32gpio->mask);
}

static void STM32GPIOClass_toggle(GPIOClass *gpio) {
	STM32GPIOClass *stm32gpio = STM32_GPIO_CLASS(gpio);
	stm32_gpioPortToggleBits(stm32gpio->port, stm32gpio->mask);
}

static uint8_t STM32GPIOClass_attachInterrupt(GPIOClass *gpio, GPIOInterruptMode mode, GPIOInterruptHandler handler, void *arg) {
	STM32GPIOClass *stm32gpio = STM32_GPIO_CLASS(gpio);
	int i;
	for (i = 0; i < 16; i++) {
		if (stm32gpio->mask & (1 << i)) {
			break;
		}
	}
	return stm32_gpioPortAttachInterrupt(stm32gpio->port, i, mode, handler, arg);
}

STM32GPIOClass *stm32_gpioClassInit(STM32GPIOClass *obj, volatile STM32GPIORegs *port, uint16_t mask) {
	GPIO_CLASS(obj)->setup = STM32GPIOClass_setup;
	GPIO_CLASS(obj)->read = STM32GPIOClass_read;
	GPIO_CLASS(obj)->set = STM32GPIOClass_set;
	GPIO_CLASS(obj)->clear = STM32GPIOClass_clear;
	GPIO_CLASS(obj)->toggle = STM32GPIOClass_toggle;
	GPIO_CLASS(obj)->attachInterrupt = STM32GPIOClass_attachInterrupt;
	obj->port = port;
	obj->mask = mask;
	return obj;
}

static void handleGPIOInterrupt(uint8_t bit) {
	if (gpioInterruptHandlers[bit].func) {
		gpioInterruptHandlers[bit].func(gpioInterruptHandlers[bit].arg);
	}
	EXTI->PR |= 1 << bit;
}

ISR(EXTI0_vect) {
	handleGPIOInterrupt(0);
}

ISR(EXTI1_vect) {
	handleGPIOInterrupt(1);
}

ISR(EXTI2_vect) {
	handleGPIOInterrupt(2);
}

ISR(EXTI3_vect) {
	handleGPIOInterrupt(3);
}

ISR(EXTI4_vect) {
	handleGPIOInterrupt(4);
}

ISR(EXTI9_5_vect) {
	for (int i = 5; i <= 9; i++) {
		if (EXTI->PR & (1 << i)) {
			handleGPIOInterrupt(i);
		}
	}
}

ISR(EXTI15_10_vect) {
	for (int i = 10; i <= 15; i++) {
		if (EXTI->PR & (1 << i)) {
			handleGPIOInterrupt(i);
		}
	}
}