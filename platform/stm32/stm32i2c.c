#include <stm32.h>

void stm32_i2cEnable(volatile STM32I2CRegs *i2c) {
	stm32_rccPeriphClockEnable(i2c);
}

void stm32_i2cDisable(volatile STM32I2CRegs *i2c) {
	stm32_rccPeriphClockDisable(i2c);
}

void stm32_i2cSetFrequency(volatile STM32I2CRegs *i2c, uint32_t frequency) {
	uint32_t clock = stm32_rccAPB1Frequency();
	i2c->CR1 = I2C1_CR1_SWRST;
	i2c->CR1 = 0;
	i2c->CR2 = clock / 1000000;
	i2c->TRISE = clock / 1000000 + 1;
	if (frequency <= 100000) { // Standard Mode
		i2c->CCR = clock / (2 * frequency);
	} else { // Fast Mode
		i2c->CCR = I2C1_CCR_F_S | (clock / (3 * frequency));
	}
	i2c->CR1 |= I2C1_CR1_PE;
}

BOOL stm32_i2cBusy(volatile STM32I2CRegs *i2c) {
	return (i2c->SR2 & I2C1_SR2_BUSY) != 0;
}

BOOL stm32_i2cWaitReady(volatile STM32I2CRegs *i2c, timestamp_t timeout) {
	timestamp_t timeoutTime = sysTickTimestamp() + timeout;
	while (stm32_i2cBusy(i2c)) {
		if (sysTickTimestamp() > timeoutTime) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL stm32_i2cWaitFlag(volatile STM32I2CRegs *i2c, uint32_t flag, timestamp_t timeout) {
	timestamp_t timeoutTime = sysTickTimestamp() + timeout;
	while ((i2c->SR1 & flag) == 0) {
		if (sysTickTimestamp() > timeoutTime) {
			return FALSE;
		}
	}
	return TRUE;
}

void stm32_i2cEnableAck(volatile STM32I2CRegs *i2c) {
	i2c->CR1 |= I2C1_CR1_ACK;
}

void stm32_i2cDisableAck(volatile STM32I2CRegs *i2c) {
	i2c->CR1 &= ~I2C1_CR1_ACK;
}

BOOL stm32_i2cMasterStart(volatile STM32I2CRegs *i2c, timestamp_t timeout) {
	i2c->CR1 |= I2C1_CR1_START;
	if (stm32_i2cWaitFlag(i2c, I2C1_SR1_SB, timeout)) {
		(void)i2c->SR1;
		return TRUE;
	}
	return FALSE;
}

BOOL stm32_i2cMasterSendAddress(volatile STM32I2CRegs *i2c, uint8_t address, BOOL oneByteRead, timestamp_t timeout) {
	i2c->DR = address;
	if (!stm32_i2cWaitFlag(i2c, I2C1_SR1_ADDR, timeout)) return FALSE;
	if (oneByteRead) {
		stm32_i2cDisableAck(i2c);
	}
	(void)i2c->SR1;
	(void)i2c->SR2;
	return TRUE;
}

BOOL stm32_i2cMasterSendByte(volatile STM32I2CRegs *i2c, uint8_t data, timestamp_t timeout) {
	i2c->DR = data;
	if (!stm32_i2cWaitFlag(i2c, I2C1_SR1_BTF, timeout)) return FALSE;
	(void)i2c->SR1;
	return TRUE;
}

int stm32_i2cMasterSendBytes(volatile STM32I2CRegs *i2c, const uint8_t *data, int count, timestamp_t timeout) {
	int i;
	for (i = 0; i < count; i++) {
		if (!stm32_i2cMasterSendByte(i2c, data[i], timeout)) break;
	}
	return i;
}

BOOL stm32_i2cMasterRecvByte(volatile STM32I2CRegs *i2c, uint8_t *buffer, BOOL lastByte, timestamp_t timeout) {
	if (lastByte) {
		stm32_i2cDisableAck(i2c);
		stm32_i2cMasterStop(i2c);
		if (!stm32_i2cWaitFlag(i2c, I2C1_SR1_RxNE, timeout)) return FALSE;
	} else {
		if (!stm32_i2cWaitFlag(i2c, I2C1_SR1_BTF, timeout)) return FALSE;
	}
	*buffer = i2c->DR;
	return TRUE;
}

int stm32_i2cMasterRecvBytes(volatile STM32I2CRegs *i2c, uint8_t *buffer, int count, timestamp_t timeout) {
	int i;
	for (i = 0; i < count; i++) {
		if (!stm32_i2cMasterRecvByte(i2c, buffer + i, i == (count - 1), timeout)) break;
	}
	return i;
}

BOOL stm32_i2cMasterTransfer(volatile STM32I2CRegs *i2c, uint8_t address, uint8_t *buffer, int count, timestamp_t timeout) {
	if (!stm32_i2cWaitReady(i2c, timeout)) return FALSE;
	if (!stm32_i2cMasterStart(i2c, timeout)) return FALSE;
	do {
		if ((address & 1) == 0) {
			if (!stm32_i2cMasterSendAddress(i2c, address, FALSE, timeout)) break;
			if (stm32_i2cMasterSendBytes(i2c, buffer, count, timeout) < count) break;
			stm32_i2cMasterStop(i2c);
			return TRUE;
		} else {
			stm32_i2cEnableAck(i2c);
			if (!stm32_i2cMasterSendAddress(i2c, address, count == 1, timeout)) break;
			if (stm32_i2cMasterRecvBytes(i2c, buffer, count, timeout) < count) break;
			return TRUE;
		}
	} while (0);
	stm32_i2cMasterStop(i2c);
	return FALSE;
}

void stm32_i2cMasterStop(volatile STM32I2CRegs *i2c) {
	i2c->CR1 |= I2C1_CR1_STOP;
}

#ifdef USE_CLASSES

static void STM32I2CClass_masterSetFrequency(I2CClass *_i2c, uint32_t frequency) {
	STM32I2CClass *i2c = STM32_I2C_CLASS(_i2c);
	i2c->frequency = frequency;
	stm32_i2cSetFrequency(i2c->regs, frequency);
}

static void STM32I2CClass_masterReset(STM32I2CClass *i2c) {
	stm32_i2cSetFrequency(i2c->regs, i2c->frequency);
}

static BOOL STM32I2CClass_waitReady(STM32I2CClass *i2c, timestamp_t timeout) {
	if (!stm32_i2cWaitReady(i2c->regs, timeout)) {
		STM32I2CClass_masterReset(i2c);
		return FALSE;
	}
	return TRUE;
}

static int STM32I2CClass_masterTransferTimeout(I2CClass *_i2c, const I2CTransfer *transfers, int transferCount, timestamp_t timeout) {
	STM32I2CClass *i2c = STM32_I2C_CLASS(_i2c);
	if (!STM32I2CClass_waitReady(i2c, timeout)) return 0;
	int i;
	for (i = 0; i < transferCount; i++) {
		const I2CTransfer *transfer = transfers + i;
		if (!stm32_i2cMasterTransfer(i2c->regs, transfer->address, transfer->buffer, transfer->count, timeout)) {
			break;
		}
	}
	if (i < transferCount) {
		stm32_i2cMasterStop(i2c->regs);
	}
	return i;
}

STM32I2CClass *stm32_i2cClassInit(STM32I2CClass *i2c, volatile STM32I2CRegs *regs, uint32_t frequency) {
	I2C_CLASS(i2c)->masterSetFrequency = STM32I2CClass_masterSetFrequency;
	I2C_CLASS(i2c)->masterTransferTimeout = STM32I2CClass_masterTransferTimeout;
	i2c->regs = regs;
	stm32_i2cEnable(regs);
	STM32I2CClass_masterSetFrequency(I2C_CLASS(i2c), frequency);
	return i2c;
}

#endif
