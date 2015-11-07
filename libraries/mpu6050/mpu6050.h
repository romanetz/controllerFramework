#ifndef __MPU6050_H__
#define __MPU6050_H__

#include <SimpleI2CDevice.h>
#include <mpu6050regs.h>

class MPU6050: public SimpleI2CDevice {
	private:
		int16_t _rawAccelX, _rawAccelY, _rawAccelZ;
		
		int16_t _rawGyroX, _rawGyroY, _rawGyroZ;
		
		int16_t _temperature;
		
	public:
		MPU6050(I2CBus& i2c, uint8_t addr);
		
		bool detect();
		
		bool powerOn();
		
		bool powerOff();
		
		bool enableI2CByPass(bool setting);
		
		bool updateData();
		
		int16_t rawAccelX() { return _rawAccelX; }
		int16_t rawAccelY() { return _rawAccelY; }
		int16_t rawAccelZ() { return _rawAccelZ; }
		
		int16_t rawGyroX() { return _rawGyroX; }
		int16_t rawGyroY() { return _rawGyroY; }
		int16_t rawGyroZ() { return _rawGyroZ; }
		
		int16_t temperature() { return _temperature; }
		
};

#endif
