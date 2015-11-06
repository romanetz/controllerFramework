#include <stm32.h>

STM32GPIOPort::STM32GPIOPort(volatile STM32GPIORegs *gpio) : _gpio(gpio) {
	STM32RCC::periphClockEnable(_gpio);
}

#ifdef STM32F1

static uint32_t makeGPIOConfig(GPIOBitMode mode, GPIOBitDriver driver, GPIOPuPdMode pupd, STM32GPIOSpeed speed) {
	uint32_t cfg = 0;
	switch (mode) {
		case GPIO_MODE_INPUT:
			if (pupd == GPIO_PUPD_NONE) {
				cfg = STM32_GPIO_MODE_INPUT | STM32_GPIO_CNF_INPUT_FLOAT;
			} else {
				cfg = STM32_GPIO_MODE_INPUT | STM32_GPIO_CNF_INPUT_PULL_UPDOWN;
			}
			break;
		case GPIO_MODE_ANALOG_INPUT:
			cfg = STM32_GPIO_MODE_INPUT | STM32_GPIO_CNF_INPUT_ANALOG;
			break;
		case GPIO_MODE_OUTPUT:
			if (driver == GPIO_DRIVER_OPENDRAIN) {
				cfg = speed | STM32_GPIO_CNF_OUTPUT_OPENDRAIN;
			} else {
				cfg = speed | STM32_GPIO_CNF_OUTPUT_PUSHPULL;
			}
			break;
		case GPIO_MODE_ALTFN:
			if (driver == GPIO_DRIVER_OPENDRAIN) {
				cfg = speed | STM32_GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN;
			} else {
				cfg = speed | STM32_GPIO_CNF_OUTPUT_ALTFN_PUSHPULL;
			}
			break;
	}
	return cfg;
}

void STM32GPIOPort::setup(GPIOBitMask bits, GPIOBitMode mode, GPIOBitDriver driver, GPIOPuPdMode pupd, STM32GPIOSpeed speed) {
	uint32_t cr[2] = {_gpio->CRL, _gpio->CRH};
	uint32_t cfg = makeGPIOConfig(mode, driver, pupd, speed);
	for (int i = 0; i < 16; i++) {
		if (bits & (1 << i)) {
			uint32_t reg = i / 8;
			uint32_t offset = (i % 8) * 4;
			cr[reg] &= ~(0xF << offset);
			cr[reg] |= cfg << offset;
		}
	}
	_gpio->CRL = cr[0];
	_gpio->CRH = cr[1];
	if (pupd == GPIO_PUPD_PULLUP) {
		_gpio->ODR |= bits;
	} else if (pupd == GPIO_PUPD_PULLDOWN) {
		_gpio->ODR &= ~bits;
	}
}

void STM32GPIOPad::setup(GPIOBitMode mode, GPIOBitDriver driver, GPIOPuPdMode pupd, STM32GPIOSpeed speed) {
	uint32_t cfg = makeGPIOConfig(mode, driver, pupd, speed);
	volatile uint32_t *reg = (_bit < 8) ? &_gpio->CRL : &_gpio->CRH;
	uint32_t offset = (_bit % 8) * 4;
	*reg &= ~(0xF << offset);
	*reg |= cfg << offset;
	if (pupd == GPIO_PUPD_PULLUP) {
		_gpio->ODR |= 1 << _bit;
	} else if (pupd == GPIO_PUPD_PULLDOWN) {
		_gpio->ODR &= ~(1 << _bit);
	}
}

#else

static uint32_t makeGPIOMode(GPIOBitMode mode) {
	switch (mode) {
		case GPIO_MODE_OUTPUT:
			return STM32_GPIO_MODE_OUTPUT;
		case GPIO_MODE_ANALOG_INPUT:
			return STM32_GPIO_MODE_ANALOG;
		case GPIO_MODE_ALTFN:
			return STM32_GPIO_MODE_AF;
		default:
			return STM32_GPIO_MODE_INPUT;
	}
}

static uint32_t makeGPIODriver(GPIOBitDriver driver) {
	return ((driver == GPIO_DRIVER_OPENDRAIN) ? STM32_GPIO_OTYPE_OD : STM32_GPIO_OTYPE_PP);
}

static uint32_t makeGPIOPuPdMode(GPIOPuPdMode pupd) {
	switch (pupd) {
		case GPIO_PUPD_PULLUP:
			return STM32_GPIO_PUPD_PULLUP;
		case GPIO_PUPD_PULLDOWN:
			return STM32_GPIO_PUPD_PULLDOWN;
		default:
			return STM32_GPIO_PUPD_NONE;
	}
}

void STM32GPIOPort::setup(GPIOBitMask bits, GPIOBitMode mode, GPIOBitDriver driver, GPIOPuPdMode pupd, STM32GPIOSpeed speed, STM32GPIOAltFn af) {
	uint32_t moder = _gpio->MODER;
	uint32_t otyper = _gpio->OTYPER;
	uint32_t ospeedr = _gpio->OSPEEDR;
	uint32_t pupdr = _gpio->PUPDR;
	uint32_t afr[2] = {_gpio->AFRL, _gpio->AFRH};
	uint32_t modeValue = makeGPIOMode(mode);
	uint32_t otype = makeGPIODriver(driver);
	uint32_t pupdValue = makeGPIOPuPdMode(pupd);
	for (int i = 0; i < 16; i++) {
		if (bits & (1 << i)) {
			moder &= ~(3 << (i * 2));
			moder |= modeValue << (i * 2);
			otyper &= ~(1 << i);
			otyper |= otype << i;
			ospeedr &= ~(3 << (i * 2));
			ospeedr |= speed << (i * 2);
			pupdr &= ~(3 << (i * 2));
			pupdr |= pupdValue << (i * 2);
			afr[i / 8] &= ~(0xF << ((i % 8) * 4));
			afr[i / 8] |= af << ((i % 8) * 4);
		}
	}
	_gpio->MODER = moder;
	_gpio->OTYPER = otyper;
	_gpio->OSPEEDR = ospeedr;
	_gpio->PUPDR = pupdr;
	_gpio->AFRL = afr[0];
	_gpio->AFRH = afr[1];
}

void STM32GPIOPad::setup(GPIOBitMode mode, GPIOBitDriver driver, GPIOPuPdMode pupd, STM32GPIOSpeed speed, STM32GPIOAltFn af) {
	uint32_t modeValue = makeGPIOMode(mode);
	uint32_t otype = makeGPIODriver(driver);
	uint32_t pupdValue = makeGPIOPuPdMode(pupd);
	_gpio->MODER &= ~(3 << (_bit * 2));
	_gpio->MODER |= modeValue << (_bit * 2);
	_gpio->OTYPER &= ~(1 << _bit);
	_gpio->OTYPER |= otype << _bit;
	_gpio->OSPEEDR &= ~(3 << (_bit * 2));
	_gpio->OSPEEDR |= speed << (_bit * 2);
	_gpio->PUPDR &= ~(3 << (_bit * 2));
	_gpio->PUPDR |= pupdValue << (_bit * 2);
	volatile uint32_t *afr = (_bit < 8) ? &_gpio->AFRL : &_gpio->AFRH;
	*afr &= ~(0xF << ((_bit % 8) * 4));
	*afr |= af << ((_bit % 8) * 4);
}

#endif
