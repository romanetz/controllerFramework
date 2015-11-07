#ifndef __SIMPLE_I2C_DEVICE_H__
#define __SIMPLE_I2C_DEVICE_H__

#include <i2c-bus.h>

class SimpleI2CDevice {
	protected:
		I2CBus& _i2c;
		
		uint32_t _addr;
		
	public:
		SimpleI2CDevice(I2CBus& i2c, uint32_t addr) : _i2c(i2c), _addr(addr) {}
		
		bool writeReg(uint8_t reg, void *data, int dataLen);
		
		bool writeByteReg(uint8_t reg, uint8_t data) {
			return writeReg(reg, &data, sizeof(data));
		}
		
		bool writeWordReg(uint8_t reg, uint16_t data) {
			return writeReg(reg, &data, sizeof(data));
		}
		
		bool readReg(uint8_t reg, void *buffer, int bufferSize);
		
		uint8_t readByteReg(uint8_t reg) {
			uint8_t value;
			if (readReg(reg, &value, sizeof(value))) {
				return value;
			}
			return 0;
		}
		
		uint16_t readWordReg(uint8_t reg) {
			uint16_t value;
			if (readReg(reg, &value, sizeof(value))) {
				return value;
			}
			return 0;
		}
		
};

#endif
