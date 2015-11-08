#ifndef __MPU6050_H__
#define __MPU6050_H__

#include <SimpleI2CDevice.h>
#include <mpu6050regs.h>

enum MPU6050GyroScale {
	MPU6050_GYROSCALE_250DEG = MPU6050_FS_SEL_250,
	MPU6050_GYROSCALE_500DEG = MPU6050_FS_SEL_500,
	MPU6050_GYROSCALE_1000DEG = MPU6050_FS_SEL_1000,
	MPU6050_GYROSCALE_2000DEG = MPU6050_FS_SEL_2000
};

enum MPU6050AccelScale {
	MPU6050_ACCELSCALE_2G = MPU6050_AFS_SEL_2G,
	MPU6050_ACCELSCALE_4G = MPU6050_AFS_SEL_4G,
	MPU6050_ACCELSCALE_8G = MPU6050_AFS_SEL_8G,
	MPU6050_ACCELSCALE_16G = MPU6050_AFS_SEL_16G
};

class MPU6050: public SimpleI2CDevice {
	private:
		int16_t _rawAccelX, _rawAccelY, _rawAccelZ;
		
		int16_t _rawGyroX, _rawGyroY, _rawGyroZ;
		
		int16_t _temperature;
		
		float _gyroScaleValue;
		
		float _accelScaleValue;
		
	public:
		MPU6050(I2CBus& i2c, uint8_t addr);
		
		bool detect();
		
		bool setup(MPU6050AccelScale accelScale = MPU6050_ACCELSCALE_2G, MPU6050GyroScale gyroScale = MPU6050_GYROSCALE_250DEG, bool i2cByPass = true);
		
		bool powerOn();
		
		bool powerOff();
		
		bool setGyroScale(MPU6050GyroScale scale);
		
		bool setAccelScale(MPU6050AccelScale scale);
		
		bool enableI2CByPass(bool setting);
		
		bool updateData();
		
		int16_t rawAccelX() { return _rawAccelX; }
		int16_t rawAccelY() { return _rawAccelY; }
		int16_t rawAccelZ() { return _rawAccelZ; }
		
		float accelX() { return _rawAccelX * _accelScaleValue; }
		float accelY() { return _rawAccelY * _accelScaleValue; }
		float accelZ() { return _rawAccelZ * _accelScaleValue; }
		
		int16_t rawGyroX() { return _rawGyroX; }
		int16_t rawGyroY() { return _rawGyroY; }
		int16_t rawGyroZ() { return _rawGyroZ; }
		
		float gyroX() { return _rawGyroX * _gyroScaleValue; }
		float gyroY() { return _rawGyroY * _gyroScaleValue; }
		float gyroZ() { return _rawGyroZ * _gyroScaleValue; }
		
		int16_t temperature() { return _temperature; }
		
};

#endif
