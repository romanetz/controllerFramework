#ifndef __STM32I2CREGS_H__
#define __STM32I2CREGS_H__

#include <stdint.h>

typedef struct STM32I2CRegs {
	uint32_t CR1; ///< Control register 1
	uint32_t CR2; ///< Control register 2
	uint32_t OAR1; ///< Own address register 1
	uint32_t OAR2; ///< Own address register 2
	uint32_t DR; ///< Data register
	uint32_t SR1; ///< Status register 1
	uint32_t SR2; ///< Status register 2
	uint32_t CCR; ///< Clock control register
	uint32_t TRISE; ///< TRISE register
} STM32I2CRegs;

#define I2C1 MMIO(I2C1_BASE, STM32I2CRegs)

#define I2C2 MMIO(I2C2_BASE, STM32I2CRegs)

#ifdef I2C3_BASE
#define I2C3 MMIO(I2C3_BASE, STM32I2CRegs)
#endif

#endif
