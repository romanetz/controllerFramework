#include <stm32.h>

STM32I2CDriver::STM32I2CDriver(volatile STM32I2CRegs *i2c) : _i2c(i2c) {
	STM32RCC::periphClockEnable(_i2c);
}
		
STM32I2CDriver::STM32I2CDriver(volatile STM32I2CRegs *i2c, uint32_t baudrate, uint32_t ownAddress1, uint32_t ownAddress2) : _i2c(i2c) {
	STM32RCC::periphClockEnable(_i2c);
	setBaudrate(baudrate);
	setOwnAddress1(ownAddress1);
	setOwnAddress2(ownAddress2);
}

bool STM32I2CDriver::setBaudrate(uint32_t baudrate) {
	_baudrate = baudrate;
	reset();
	return true;
}

void STM32I2CDriver::reset() {
	uint32_t clock = STM32RCC::apb1Frequency();
	_i2c->CR1 = I2C1_CR1_SWRST;
	_i2c->CR1 = 0;
	_i2c->CR2 = clock / 1000000;
	_i2c->TRISE = clock / 1000000 + 1;
	if (_baudrate <= 100000) { // Standard Mode
		_i2c->CCR = clock / (2 * _baudrate);
	} else { // Fast Mode
		_i2c->CCR = I2C1_CCR_F_S | (clock / (3 * _baudrate));
	}
	_i2c->CR1 |= I2C1_CR1_PE;
}

bool STM32I2CDriver::waitBusy(uint64_t timeout) {
	uint64_t timeoutTime = SysTick::timestamp() + timeout;
	while (_i2c->SR2 & I2C1_SR2_BUSY) {
		if (SysTick::timestamp() >= timeoutTime) {
			reset();
			return false;
		}
	}
	return true;
}

bool STM32I2CDriver::waitFlag(uint32_t flag, uint64_t timeout) {
	uint64_t timeoutTime = SysTick::timestamp() + timeout;
	while ((_i2c->SR1 & flag) == 0) {
		if (SysTick::timestamp() >= timeoutTime) {
			return false;
		}
	}
	return true;
}

int STM32I2CDriver::masterTransaction(const I2CTransaction *transaction, uint64_t timeout) {
	unsigned int i;
	if (!waitBusy(timeout)) return 0;
	_i2c->CR1 |= I2C1_CR1_START;
	for (i = 0; i < transaction->messageCount; i++) {
		if (!waitFlag(I2C1_SR1_SB, timeout)) break;
		(void)_i2c->SR1;
		uint32_t nextAction = (i == (transaction->messageCount - 1)) ? I2C1_CR1_STOP : I2C1_CR1_START;
		const I2CMessage *msg = transaction->messages + i;
		uint8_t *buffer = (uint8_t*)msg->buffer;
		if (msg->write) {
			_i2c->DR = transaction->slaveAddress << 1;
			if (!waitFlag(I2C1_SR1_ADDR, timeout)) break;
			(void)_i2c->SR1;
			(void)_i2c->SR2;
			for (unsigned int j = 0; j < msg->bufferSize; j++) {
				_i2c->DR = buffer[j];
				if (!waitFlag(I2C1_SR1_BTF, timeout)) goto masterTransactionTimeout;
				(void)_i2c->SR1;
			}
			_i2c->CR1 |= nextAction;
		} else {
			_i2c->CR1 |= I2C1_CR1_ACK;
			_i2c->DR = (transaction->slaveAddress << 1) | 1;
			if (!waitFlag(I2C1_SR1_ADDR, timeout)) break;
			if (msg->bufferSize == 1) {
				_i2c->CR1 &= ~I2C1_CR1_ACK;
			}
			(void)_i2c->SR1;
			(void)_i2c->SR2;
			for (unsigned int j = 0; j < msg->bufferSize; j++) {
				if (msg->bufferSize - j == 1) { // Last Byte
					_i2c->CR1 &= ~I2C1_CR1_ACK;
					_i2c->CR1 |= nextAction;
					if (!waitFlag(I2C1_SR1_RxNE, timeout)) goto masterTransactionTimeout;
				} else {
					if (!waitFlag(I2C1_SR1_BTF, timeout)) goto masterTransactionTimeout;
				}
				buffer[j] = _i2c->DR;
			}
		}
	}
	return i;
masterTransactionTimeout:
	_i2c->CR1 |= I2C1_CR1_STOP;
	return i;
}

