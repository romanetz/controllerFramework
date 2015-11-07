#include <i2c-bus.h>

int I2CBus::masterTxRx(uint16_t slaveAddress, const void *txBuffer, unsigned int txLen, void *rxBuffer, unsigned int rxLen, uint64_t timeout) {
	I2CMessage messages[2];
	messages[0].write = true;
	messages[0].buffer = (void*)txBuffer;
	messages[0].bufferSize = txLen;
	messages[1].write = false;
	messages[1].buffer = rxBuffer;
	messages[1].bufferSize = rxLen;
	I2CTransaction transaction;
	transaction.slaveAddress = slaveAddress;
	transaction.messageCount = (rxBuffer && rxLen) ? 2 : 1;
	transaction.messages = messages;
	return masterTransaction(&transaction, timeout);
}

