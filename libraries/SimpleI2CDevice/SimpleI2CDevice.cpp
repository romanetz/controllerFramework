#include <string.h>
#include <SimpleI2CDevice.h>

bool SimpleI2CDevice::writeReg(uint8_t reg, void *data, int dataLen) {
	uint8_t txBuf[dataLen + 1];
	txBuf[0] = reg;
	memmove(txBuf + 1, data, dataLen);
	return _i2c.masterTxRx(_addr, txBuf, sizeof(txBuf));
}

bool SimpleI2CDevice::readReg(uint8_t reg, void *buffer, int bufferSize) {
	uint8_t txBuf[] = {reg};
	return _i2c.masterTxRx(_addr, txBuf, sizeof(txBuf), buffer, bufferSize) == 2;
}

bool SimpleI2CDevice::writeByteReg(uint8_t reg, uint8_t data) {
	return writeReg(reg, &data, sizeof(data));
}

bool SimpleI2CDevice::writeWordReg(uint8_t reg, uint16_t data) {
	return writeReg(reg, &data, sizeof(data));
}

uint8_t SimpleI2CDevice::readByteReg(uint8_t reg) {
	uint8_t value;
	if (readReg(reg, &value, sizeof(value))) {
		return value;
	}
	return 0;
}
		
uint16_t SimpleI2CDevice::readWordReg(uint8_t reg) {
	uint16_t value;
	if (readReg(reg, &value, sizeof(value))) {
		return value;
	}
	return 0;
}
