#ifndef __STM32RCCREGS_H__
#define __STM32RCCREGS_H__

typedef struct STM32RCCRegs {
#if defined(STM32F1) || defined(STM32F2)
	uint32_t CR;
	uint32_t CFGR;
	uint32_t CIR;
	uint32_t APB2RSTR;
	uint32_t APB1RSTR;
	uint32_t AHBENR;
	uint32_t APB2ENR;
	uint32_t APB1ENR;
	uint32_t BDCR;
	uint32_t CSR;
	uint32_t AHBRSTR;
	uint32_t CFGR2;
#ifdef STM32F2
	uint32_t CFGR3;
#endif
#else
	uint32_t CR;
	uint32_t PLLCFGR;
	uint32_t CFGR;
	uint32_t CIR;
	uint32_t AHB1RSTR;
	uint32_t AHB2RSTR;
	uint32_t AHB3RSTR;
	uint32_t RESERVED1;
	uint32_t APB1RSTR;
	uint32_t APB2RSTR;
	uint32_t RESERVED2;
	uint32_t RESERVED3;
	uint32_t AHB1ENR;
	uint32_t AHB2ENR;
	uint32_t AHB3ENR;
	uint32_t RESERVED4;
	uint32_t APB1ENR;
	uint32_t APB2ENR;
	uint32_t RESERVED5;
	uint32_t RESERVED6;
	uint32_t AHB1LPENR;
	uint32_t AHB2LPENR;
	uint32_t AHB3LPENR;
	uint32_t RESERVED7;
	uint32_t APB1LPENR;
	uint32_t APB2LPENR;
	uint32_t RESERVED8;
	uint32_t RESERVED9;
	uint32_t BDCR;
	uint32_t CSR;
	uint32_t RESERVED10;
	uint32_t RESERVED11;
	uint32_t SSCGR;
	uint32_t PLLI2SCFGR;
#ifndef STM32F4
	uint32_t PLLSAICFGR;
	uint32_t DCKCFGR;
#endif
#endif
} STM32RCCRegs;

typedef enum STM32APBPrescaler {
	RCC_APB_NODIV = 0,
	RCC_APB_DIV2 = 4,
	RCC_APB_DIV4 = 5,
	RCC_APB_DIV8 = 6,
	RCC_APB_DIV16 = 7
} STM32APBPrescaler;

typedef enum STM32AHBPrescaler {
	RCC_AHB_NODIV = 0,
	RCC_AHB_DIV2 = 8,
	RCC_AHB_DIV4 = 9,
	RCC_AHB_DIV8 = 10,
	RCC_AHB_DIV16 = 11,
	RCC_AHB_DIV64 = 12,
	RCC_AHB_DIV128 = 13,
	RCC_AHB_DIV256 = 14,
	RCC_AHB_DIV512 = 15
} STM32AHBPrescaler;

#ifdef STM32F1
typedef enum STM32ADCPrescaler {
	STM32_ADCPRESCALER_DIV2 = 0,
	STM32_ADCPRESCALER_DIV4 = 1,
	STM32_ADCPRESCALER_DIV6 = 2,
	STM32_ADCPRESCALER_DIV8 = 3
} STM32ADCPrescaler;
#endif

#endif
