#include <i2c.h>

#ifdef USE_CLASSES

int i2cMasterWriteReadTimeout(void *i2c, int address, void *txBuffer, int txCount, void *rxBuffer, int rxCount, uint64_t timeout) {
	int transferCount = 1;
	I2CTransfer transfers[2];
	transfers[0].address = address;
	transfers[0].count = txCount;
	transfers[0].buffer = txBuffer;
	if (rxCount > 0) {
		transferCount++;
		transfers[1].address = address | 1;
		transfers[1].count = rxCount;
		transfers[1].buffer = rxBuffer;
	}
	return i2cMasterTransferTimeout(i2c, transfers, transferCount, timeout);
}

#endif