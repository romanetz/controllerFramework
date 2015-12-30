#ifndef __STM32_GPIO_REGS_H__
#define __STM32_GPIO_REGS_H__

typedef struct STM32GPIORegs {
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
} STM32GPIORegs;

#ifdef AFIO_BASE
typedef struct STM32AFIORegs {
	uint32_t EVCR;
	uint32_t MAPR;
	uint32_t EXTICR[4];
	uint32_t MAPR2;
} STM32AFIORegs;
#endif

#ifdef SYSCFG_BASE
typedef struct STM32SYSCFGRegs {
	uint32_t MEMRM;
	uint32_t PMC;
	uint32_t EXTICR[4];
	uint32_t CMPCR;
} STM32SYSCFGRegs;
#endif

typedef struct STM32EXTIRegs {
	uint32_t IMR;
	uint32_t EMR;
	uint32_t RTSR;
	uint32_t FTSR;
	uint32_t SWIER;
	uint32_t PR;
} STM32EXTIRegs;

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

typedef enum STM32GPIOMode {
	STM32_GPIO_MODE_INPUT = 0, ///< Digital input
	STM32_GPIO_MODE_OUTPUT = 1, ///< Digital output
	STM32_GPIO_MODE_AF = 2, ///< Alternative function (input or output)
	STM32_GPIO_MODE_ANALOG = 3 ///< Analog input
} STM32GPIOMode;

typedef enum STM32GPIOOType {
	STM32_GPIO_OTYPE_PP = 0, ///< Push-pull output
	STM32_GPIO_OTYPE_OD = 1 ///< Open-drain output
} STM32GPIOOType;

typedef enum STM32GPIOPuPd {
	STM32_GPIO_PUPD_NONE = 0, ///< Disable pullup/pulldown
	STM32_GPIO_PUPD_PULLUP = 1, ///< Pullup to VCC
	STM32_GPIO_PUPD_PULLDOWN = 2 ///< Pulldown to GND
} STM32GPIOPuPd;

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

typedef enum STM32GPIOSpeed {
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
} STM32GPIOSpeed;

#endif
