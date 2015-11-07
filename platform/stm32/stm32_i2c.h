/**
	\addtogroup STM32 STM32
	@{
	\addtogroup STM32_I2C I2C
	@{
*/
#ifndef __STM32_I2C_H__
#define __STM32_I2C_H__

#include <stm32.h>
#include <i2c-bus.h>

/**
	\brief STM32 I2C peripheral MMIO registers
*/
struct STM32I2CRegs {
	uint32_t CR1; ///< Control register 1
	uint32_t CR2; ///< Control register 2
	uint32_t OAR1; ///< Own address register 1
	uint32_t OAR2; ///< Own address register 2
	uint32_t DR; ///< Data register
	uint32_t SR1; ///< Status register 1
	uint32_t SR2; ///< Status register 2
	uint32_t CCR; ///< Clock control register
	uint32_t TRISE; ///< TRISE register
};

#define I2C1 MMIO(I2C1_BASE, STM32I2CRegs)

#define I2C2 MMIO(I2C2_BASE, STM32I2CRegs)

#ifdef I2C3_BASE
#define I2C3 MMIO(I2C3_BASE, STM32I2CRegs)
#endif

/**
	\brief STM32 I2C peripheral class
*/
class STM32I2CDriver: public I2CBus {
	private:
		volatile STM32I2CRegs *_i2c;
		
		uint32_t _baudrate;
		
	public:
		STM32I2CDriver(volatile STM32I2CRegs *i2c);
		
		STM32I2CDriver(volatile STM32I2CRegs *i2c, uint32_t baudrate, uint32_t ownAddress1 = 0, uint32_t ownAddress2 = 0);
		
		bool setBaudrate(uint32_t baudrate);
		
		void setOwnAddress1(uint32_t address) {
			_i2c->OAR1 = address;
		}
		
		void setOwnAddress2(uint32_t address) {
			_i2c->OAR2 = address;
		}
		
		int masterTransaction(const I2CTransaction *transaction, uint64_t timeout);
		
	private:
		void reset();
		
		bool waitBusy(uint64_t timeout);
		
		bool waitFlag(uint32_t flag, uint64_t timeout);
		
};

#endif
/**
	@}
	@}
*/
