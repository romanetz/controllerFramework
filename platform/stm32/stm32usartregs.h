#ifndef __STM32USARTREGS_H__ 
#define __STM32USARTREGS_H__

#include <stm32.h>

typedef struct STM32USARTRegs {
	uint32_t SR; ///< Status register
	uint32_t DR; ///< Data register
	uint32_t BRR; ///< Baudrate register
	uint32_t CR1; ///< Control register 1
	uint32_t CR2; ///< Control register 2
	uint32_t CR3; ///< Control register 3
	uint32_t GTRR; ///< Guard time and prescaler register	
} STM32USARTRegs;

#define USART1 MMIO(USART1_BASE, STM32USARTRegs)

#define USART2 MMIO(USART2_BASE, STM32USARTRegs)

#define USART3 MMIO(USART3_BASE, STM32USARTRegs)

#ifdef UART4_BASE
#define UART4 MMIO(UART4_BASE, STM32USARTRegs)
#endif

#ifdef UART5_BASE
#define UART5 MMIO(UART5_BASE, STM32USARTRegs)
#endif

#ifdef USART6_BASE
#define USART6 MMIO(USART6_BASE, STM32USARTRegs)
#endif

#endif