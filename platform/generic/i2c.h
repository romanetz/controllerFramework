#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>

typedef struct I2CTransfer {
	int address;
	int count;
	uint8_t *buffer;
} I2CTransfer;

#ifdef USE_CLASSES

typedef struct I2CClass I2CClass;

typedef int (*I2CClassMasterTransferTimeoutFunc)(I2CClass *i2c, I2CTransfer *transfers, int transferCount, uint64_t timeout);

struct I2CClass {
	I2CClassMasterTransferTimeoutFunc masterTransferTimeout;
};

#define I2C_CLASS(obj) ((I2CClass*)(obj))

#define i2cMasterTransferTimeout(i2c, transfers, transferCount, timeout) I2C_CLASS(i2c)->masterTransferTimeout(I2C_CLASS(i2c), transfers, transferCount, timeout)
#define i2cMasterTransfer(i2c, transfers, transferCount) i2cMasterTransferTimeout(i2c, transfers, transferCount, -1)

int i2cMasterWriteReadTimeout(void *i2c, int address, void *txBuffer, int txCount, void *rxBuffer, int rxCount, uint64_t timeout);
#define i2cMasterWriteRead(i2c, address, txBuffer, txCount, rxBuffer, rxCount) i2cMasterWriteReadTimeout(i2c, address, txBuffer, txCount, rxBuffer, rxCount, -1)

#endif

#endif
