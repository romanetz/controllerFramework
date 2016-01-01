#include <i2c.h>

#ifdef USE_CLASSES

BOOL i2cMasterWriteReadTimeout(void *i2c, int address, const void *txBuffer, int txCount, void *rxBuffer, int rxCount, timestamp_t timeout) {
	int transferCount = 1;
	I2CTransfer transfers[2];
	transfers[0].address = address;
	transfers[0].count = txCount;
	transfers[0].buffer = (uint8_t*)txBuffer;
	if (rxCount > 0) {
		transferCount++;
		transfers[1].address = address | 1;
		transfers[1].count = rxCount;
		transfers[1].buffer = (uint8_t*)rxBuffer;
	}
	return i2cMasterTransferTimeout(i2c, transfers, transferCount, timeout) == transferCount;
}

#endif