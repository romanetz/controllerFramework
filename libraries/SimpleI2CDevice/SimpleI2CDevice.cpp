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

