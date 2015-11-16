/**
	\addtogroup STM32 STM32
	@{
	\addtogroup STM32_GPIO GPIO
	@{
*/
#ifndef __STM32_GPIO_H__
#define __STM32_GPIO_H__

#include <stm32.h>
#include <gpio.h>

/**
	\brief STM32 GPIO MMIO registers.
*/
struct STM32GPIORegs {
#ifdef STM32F1
	uint32_t CRL;
	uint32_t CRH;
	uint32_t IDR;
	uint32_t ODR;
	uint32_t BSRR;
	uint32_t BRR;
	uint32_t LCKR;
#else
	uint32_t MODER;
	uint32_t OTYPER;
	uint32_t OSPEEDR;
	uint32_t PUPDR;
	uint32_t IDR;
	uint32_t ODR;
	uint32_t BSRR;
	uint32_t LCKR;
	uint32_t AFRL;
	uint32_t AFRH;
#endif
};

#ifdef AFIO_BASE
struct STM32AFIORegs {
	uint32_t EVCR;
	uint32_t MAPR;
	uint32_t EXTICR[4];
	uint32_t MAPR2;
};
#endif

#ifdef SYSCFG_BASE
struct STM32SYSCFGRegs {
	uint32_t MEMRM;
	uint32_t PMC;
	uint32_t EXTICR[4];
	uint32_t CMPCR;
};
#endif

struct STM32EXTIRegs {
	uint32_t IMR;
	uint32_t EMR;
	uint32_t RTSR;
	uint32_t FTSR;
	uint32_t SWIER;
	uint32_t PR;
};

#ifdef AFIO_BASE
#define AFIO MMIO(AFIO_BASE, STM32AFIORegs)
#endif

#ifdef SYSCFG_BASE
#define SYSCFG MMIO(SYSCFG_BASE, STM32SYSCFGRegs)
#endif

#define EXTI MMIO(EXTI_BASE, STM32EXTIRegs)

#ifdef STM32F1

#define STM32_GPIO_MODE_INPUT 0x00
#define STM32_GPIO_MODE_OUTPUT_10_MHZ 0x01
#define STM32_GPIO_MODE_OUTPUT_2_MHZ 0x02
#define STM32_GPIO_MODE_OUTPUT_50_MHZ 0x03

#define STM32_GPIO_CNF_INPUT_ANALOG 0x00
#define STM32_GPIO_CNF_INPUT_FLOAT 0x04
#define STM32_GPIO_CNF_INPUT_PULL_UPDOWN 0x08
#define STM32_GPIO_CNF_OUTPUT_PUSHPULL 0x00
#define STM32_GPIO_CNF_OUTPUT_OPENDRAIN 0x04
#define STM32_GPIO_CNF_OUTPUT_ALTFN_PUSHPULL 0x08
#define STM32_GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN 0x0C

#else

/**
	\brief Values for STM32 GPIO MODER register.
*/
enum STM32GPIOMode {
	STM32_GPIO_MODE_INPUT = 0, ///< Digital input
	STM32_GPIO_MODE_OUTPUT = 1, ///< Digital output
	STM32_GPIO_MODE_AF = 2, ///< Alternative function (input or output)
	STM32_GPIO_MODE_ANALOG = 3 ///< Analog input
};

/**
	\brief Values for STM32 GPIO OTYPER register.
*/
enum STM32GPIOOType {
	STM32_GPIO_OTYPE_PP = 0, ///< Push-pull output
	STM32_GPIO_OTYPE_OD = 1 ///< Open-drain output
};

/**
	\brief Values for STM32 GPIO PUPDR register.
*/
enum STM32GPIOPuPd {
	STM32_GPIO_PUPD_NONE = 0, ///< Disable pullup/pulldown
	STM32_GPIO_PUPD_PULLUP = 1, ///< Pullup to VCC
	STM32_GPIO_PUPD_PULLDOWN = 2 ///< Pulldown to GND
};

#endif

#define GPIOA MMIO(GPIOA_BASE, STM32GPIORegs)

#define GPIOB MMIO(GPIOB_BASE, STM32GPIORegs)

#define GPIOC MMIO(GPIOC_BASE, STM32GPIORegs)

#define GPIOD MMIO(GPIOD_BASE, STM32GPIORegs)

#ifdef GPIOE_BASE
#define GPIOE MMIO(GPIOE_BASE, STM32GPIORegs)
#endif

#ifdef GPIOF_BASE
#define GPIOF MMIO(GPIOF_BASE, STM32GPIORegs)
#endif

#ifdef GPIOG_BASE
#define GPIOG MMIO(GPIOG_BASE, STM32GPIORegs)
#endif

#ifdef GPIOH_BASE
#define GPIOH MMIO(GPIOH_BASE, STM32GPIORegs)
#endif

#ifdef GPIOI_BASE
#define GPIOI MMIO(GPIOI_BASE, STM32GPIORegs)
#endif

/**
	\brief STM32 GPIO speed modes.
*/
enum STM32GPIOSpeed {
#ifdef STM32F1
	STM32_GPIO_SPEED_2MHZ = STM32_GPIO_MODE_OUTPUT_2_MHZ, ///< Output speed up to 2 MHz
	STM32_GPIO_SPEED_10MHZ = STM32_GPIO_MODE_OUTPUT_10_MHZ, ///< Output speed up to 10 MHz
	STM32_GPIO_SPEED_50MHZ = STM32_GPIO_MODE_OUTPUT_50_MHZ, ///< Output speed up to 50 MHz
	
	STM32_GPIO_SPEED_MAX = STM32_GPIO_SPEED_50MHZ ///< Maximal output speed
#else
	STM32_GPIO_SPEED_2MHZ =	0, ///< Output speed up to 2 MHz
	STM32_GPIO_SPEED_25MHZ = 1, ///< Output speed up to 25 MHz
	STM32_GPIO_SPEED_50MHZ = 2, ///< Output speed up to 50 MHz
	STM32_GPIO_SPEED_100MHZ = 3, ///< Output speed up to 100 MHz

	STM32_GPIO_SPEED_MAX = STM32_GPIO_SPEED_100MHZ ///< Maximal output speed
#endif
};

#ifndef STM32F1
/**
	\brief STM32 GPIO alternative function numbers.
*/
enum STM32GPIOAltFn {
	STM32_AF_NONE = -1, ///< Used for GPIO with mode != GPIO_MODE_ALTFN

	STM32_AF0 = 0, ///< Alternative function 0
	STM32_AF1 = 1, ///< Alternative function 1
	STM32_AF2 = 2, ///< Alternative function 2
	STM32_AF3 = 3, ///< Alternative function 3
	STM32_AF4 = 4, ///< Alternative function 4
	STM32_AF5 = 5, ///< Alternative function 5
	STM32_AF6 = 6, ///< Alternative function 6
	STM32_AF7 = 7, ///< Alternative function 7
	STM32_AF8 = 8, ///< Alternative function 8
	STM32_AF9 = 9, ///< Alternative function 9
	STM32_AF10 = 10, ///< Alternative function 10
	STM32_AF11 = 11, ///< Alternative function 11
	STM32_AF12 = 12, ///< Alternative function 12
	STM32_AF13 = 13, ///< Alternative function 13
	STM32_AF14 = 14, ///< Alternative function 14
	STM32_AF15 = 15 ///< Alternative function 15
};
#endif

/**
	\brief STM32 GPIO port class.
*/
class STM32GPIOPort : public GPIOPort {
	private:
		volatile STM32GPIORegs *_gpio;
		
	public:
		STM32GPIOPort(volatile STM32GPIORegs *gpio);
		
		void setup(GPIOBitMask bits, GPIOBitMode mode, GPIOBitDriver driver = GPIO_DRIVER_PUSHPULL, GPIOPuPdMode pupd = GPIO_PUPD_NONE) {
			setup(bits, mode, driver, pupd, STM32_GPIO_SPEED_MAX);
		}

#ifdef STM32F1
		void setup(GPIOBitMask bits, GPIOBitMode mode, GPIOBitDriver driver, GPIOPuPdMode pupd, STM32GPIOSpeed speed);
#else
		void setup(GPIOBitMask bits, GPIOBitMode mode, GPIOBitDriver driver, GPIOPuPdMode pupd, STM32GPIOSpeed speed, STM32GPIOAltFn af = STM32_AF_NONE);
#endif
		
		void write(GPIOBitMask data) {
			_gpio->ODR = data;
		}
		
		void setBits(GPIOBitMask bits) {
			_gpio->ODR |= bits;
		}
		
		void clearBits(GPIOBitMask bits) {
			_gpio->ODR &= ~bits;
		}
		
		void toggleBits(GPIOBitMask bits) {
			_gpio->ODR ^= bits;
		}
		
		GPIOBitMask read() {
			return _gpio->IDR;
		}
		
		GPIOBitMask readBits(GPIOBitMask bits) {
			return _gpio->IDR & bits;
		}
		
		void lockBits(GPIOBitMask bits) {
			_gpio->LCKR |= bits;
		}
		
		/**
			\brief Get GPIO port MMIO registers base address.
		*/
		volatile STM32GPIORegs *mmioRegs() const {
			return _gpio;
		}
		
		bool attachInterrupt(GPIOBitIndex bit, GPIOInterruptMode mode, GPIOInterruptHandler handler, void *arg);
		
};

/**
	\brief STM32 single GPIO pad class.
*/
class STM32GPIOPad : public GPIOPad {
	private:
		volatile STM32GPIORegs *_gpio;
		
		const GPIOBitIndex _bit;
	
	public:
		STM32GPIOPad(STM32GPIORegs *gpio, GPIOBitIndex bit) : _gpio(gpio), _bit(bit) {}
		
		STM32GPIOPad(const STM32GPIOPort& port, GPIOBitIndex bit) : _gpio(port.mmioRegs()), _bit(bit) {}
		
#ifdef STM32F1
		STM32GPIOPad(STM32GPIORegs *gpio, GPIOBitIndex bit, GPIOBitMode mode, GPIOBitDriver driver = GPIO_DRIVER_PUSHPULL,
				GPIOPuPdMode pupd = GPIO_PUPD_NONE, STM32GPIOSpeed speed = STM32_GPIO_SPEED_MAX) : _gpio(gpio), _bit(bit) {
			setup(mode, driver, pupd, speed);
		}
		
		STM32GPIOPad(const STM32GPIOPort& port, GPIOBitIndex bit, GPIOBitMode mode, GPIOBitDriver driver = GPIO_DRIVER_PUSHPULL,
				GPIOPuPdMode pupd = GPIO_PUPD_NONE, STM32GPIOSpeed speed = STM32_GPIO_SPEED_MAX) : _gpio(port.mmioRegs()), _bit(bit) {
			setup(mode, driver, pupd, speed);
		}
		
		void setup(GPIOBitMode mode, GPIOBitDriver driver, GPIOPuPdMode pupd, STM32GPIOSpeed speed);
		
#else
		STM32GPIOPad(STM32GPIORegs *gpio, GPIOBitIndex bit, GPIOBitMode mode, GPIOBitDriver driver = GPIO_DRIVER_PUSHPULL,
				GPIOPuPdMode pupd = GPIO_PUPD_NONE, STM32GPIOSpeed speed = STM32_GPIO_SPEED_MAX,
				STM32GPIOAltFn af = STM32_AF_NONE) : _gpio(gpio), _bit(bit) {
			setup(mode, driver, pupd, speed, af);
		}
		
		STM32GPIOPad(const STM32GPIOPort& port, GPIOBitIndex bit, GPIOBitMode mode, GPIOBitDriver driver = GPIO_DRIVER_PUSHPULL,
				GPIOPuPdMode pupd = GPIO_PUPD_NONE, STM32GPIOSpeed speed = STM32_GPIO_SPEED_MAX,
				STM32GPIOAltFn af = STM32_AF_NONE) : _gpio(port.mmioRegs()), _bit(bit) {
			setup(mode, driver, pupd, speed, af);
		}
		
		void setup(GPIOBitMode mode, GPIOBitDriver driver, GPIOPuPdMode pupd, STM32GPIOSpeed speed, STM32GPIOAltFn af = STM32_AF_NONE);
		
#endif
		void setup(GPIOBitMode mode, GPIOBitDriver driver = GPIO_DRIVER_PUSHPULL, GPIOPuPdMode pupd = GPIO_PUPD_NONE) {
			setup(mode, driver, pupd, STM32_GPIO_SPEED_MAX);
		}

		void set() {
			_gpio->ODR |= 1 << _bit;
		}
		
		void clear() {
			_gpio->ODR &= ~(1 << _bit);
		}
		
		void toggle() {
			_gpio->ODR ^= 1 << _bit;
		}
		
		void lock() {
			_gpio->LCKR |= 1 << _bit;
		}
		
		GPIOBitValue read() {
			return ((_gpio->IDR & (1 << _bit)) == 0) ? GPIO_PAD_LOW : GPIO_PAD_HIGH;
		}
		
		bool attachInterrupt(GPIOInterruptMode mode, GPIOInterruptHandler handler, void *arg);
		
};

#endif
/** @} */
