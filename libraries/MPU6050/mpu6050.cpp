#include <mpu6050.h>

struct MPU6050DataRegs {
	int16_t accelX, accelY, accelZ;
	int16_t temperature;
	int16_t gyroX, gyroY, gyroZ;
} __attribute__((packed));

MPU6050::MPU6050(I2CBus& i2c, uint8_t addr) : SimpleI2CDevice(i2c, addr) {
	//Do nothing
}

bool MPU6050::detect() {
	if (readByteReg(MPU6050_WHO_AM_I) == MPU6050_DEFAULT_ADDRESS) {
		return true;
	}
	return false;
}

bool MPU6050::setup(MPU6050AccelScale accelScale, MPU6050GyroScale gyroScale, bool i2cByPass) {
	if (!powerOn()) return false;
	if (!setAccelScale(accelScale)) return false;
	if (!setGyroScale(gyroScale)) return false;
	if (!enableI2CByPass(i2cByPass)) return false;
	return true;
}

bool MPU6050::powerOn() {
	return clearByteRegBits(MPU6050_PWR_MGMT_1, (1 << MPU6050_DEVICE_RESET) | (1 << MPU6050_SLEEP));
}

bool MPU6050::powerOff() {
	return setByteRegBits(MPU6050_PWR_MGMT_1, 1 << MPU6050_SLEEP);
}

bool MPU6050::setGyroScale(MPU6050GyroScale scale) {
	if (!writeByteReg(MPU6050_GYRO_CONFIG, scale)) return false;
	switch (scale) {
		case MPU6050_GYROSCALE_250DEG:
			_gyroScaleValue = 1.0f / 131.0f;
			break;
		case MPU6050_GYROSCALE_500DEG:
			_gyroScaleValue = 1.0f / 65.5f;
			break;
		case MPU6050_GYROSCALE_1000DEG:
			_gyroScaleValue = 1.0f / 32.8f;
			break;	
		case MPU6050_GYROSCALE_2000DEG:
			_gyroScaleValue = 1.0f / 16.4f;
			break;
	}
	return true;
}
		
bool MPU6050::setAccelScale(MPU6050AccelScale scale) {
	if (!writeByteReg(MPU6050_ACCEL_CONFIG, scale)) return false;
	switch (scale) {
		case MPU6050_ACCELSCALE_2G:
			_accelScaleValue = 1.0f / 16384.0f;
			break;
		case MPU6050_ACCELSCALE_4G:
			_accelScaleValue = 1.0f / 8192.0f;
			break;
		case MPU6050_ACCELSCALE_8G:
			_accelScaleValue = 1.0f / 4096.0f;
			break;
		case MPU6050_ACCELSCALE_16G:
			_accelScaleValue = 1.0f / 2048.0f;
			break;
	}
	return true;
}

bool MPU6050::enableI2CByPass(bool setting) {
	if (setting) {
		return setByteRegBits(MPU6050_INT_PIN_CFG, 1 << MPU6050_I2C_BYPASS_EN);
	} else {
		return clearByteRegBits(MPU6050_INT_PIN_CFG, 1 << MPU6050_I2C_BYPASS_EN);
	}
}

bool MPU6050::updateData() {
	volatile MPU6050DataRegs data;
	if (!readReg(MPU6050_ACCEL_XOUT_H, (void*)&data, sizeof(data))) return false;
	uint16_t *rawData = (uint16_t*)&data;
	for (unsigned int i = 0; i < (sizeof(data) / sizeof(uint16_t)); i++) {
		uint16_t value = rawData[i];
		rawData[i] = ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
	}
	_rawAccelX = data.accelX;
	_rawAccelY = data.accelY;
	_rawAccelZ = data.accelZ;
	_rawGyroX = data.gyroX;
	_rawGyroY = data.gyroY;
	_rawGyroZ = data.gyroZ;
	_temperature = (int32_t)data.temperature * 100 / 340 + 3653;
	return true;
}
