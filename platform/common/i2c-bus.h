/**
	\addtogroup platform Platform-independent
	@{
	\addtogroup I2C I2C
	@{
*/
#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>

/**
	\brief One I2C message
*/
struct I2CMessage {
	bool write; ///< If true this is write transaction, otherwise read
	void *buffer; ///< Pointer to TX/RX buffer
	unsigned int bufferSize; ///< Length of data to transmit or receive
};

/**
	\brief I2C bus transaction information
*/
struct I2CTransaction {
	uint16_t slaveAddress; ///< Slave address
	unsigned int messageCount; ///< Count of messages
	const I2CMessage *messages; ///< Pointer to array of I2C messages
};

/**
	\brief Basic class for I2C interface implementations.
*/
class I2CBus {
	public:
		/**
			\brief Set I2C bus baudrate.
			\param[in] baudrate Desired bus baudrate.
			\return Returns true if baudrate sucessfully changed or false if baudrate is not supported by the hardware.
			Common baudrates are 100000 (standard mode) and 400000 (fast mode).
			Some implementations may ignore this setting and have only one supported baudrate.
		*/
		virtual bool setBaudrate(uint32_t baudrate) = 0;
		
		/**
			\brief Execute data exchange transaction in master mode.
			\param[in] transaction Pointer to structure information about transaction.
			\param[in] timeout Timeout value in microseconds.
			\return Return number of messages that sucessfully processed.
		*/
		virtual int masterTransaction(const I2CTransaction *transaction, uint64_t timeout);
		
		/**
			\brief Transmit data in master mode and optionally receive response.
			\param[in] slaveAddress I2C address of the slave device.
			\param[in] txBuffer Pointer to buffer with data to transmit.
			\param[in] txLen Number of bytes to transmit.
			\param[in] rxBuffer Pointer to buffer for received data (can be NULL if no data receive need).
			\param[in] rxLen Count of bytes that will be received (ignored if rxBuffer == NULL).
			\param[in] timeout Timeout value in microseconds.
			\return Returns 0 if transmit error, 1 if receive error or 2 if both operations sucessfully completed.
		*/
		int masterTxRx(uint16_t slaveAddress, const void *txBuffer, unsigned int txLen,
			void *rxBuffer = 0, unsigned int rxLen = 0, uint64_t timeout = 10000);
};

#endif
/**
	@}
	@}
*/
