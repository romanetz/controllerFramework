#include <mpu6050.h>

struct MPU6050DataRegs {
	int16_t accelX, accelY, accelZ;
	int16_t temperature;
	int16_t gyroX, gyroY, gyroZ;
} __attribute__((packed));

MPU6050::MPU6050(I2CBus& i2c, uint8_t addr) : SimpleI2CDevice(i2c, addr) {
	/* if (detect()) {
		powerOn();
	} */
}

bool MPU6050::detect() {
	if (readByteReg(MPU6050_WHO_AM_I) == MPU6050_DEFAULT_ADDRESS) {
		return true;
	}
	return false;
}

bool MPU6050::powerOn() {
	return clearByteRegBits(MPU6050_PWR_MGMT_1, (1 << MPU6050_DEVICE_RESET) | (1 << MPU6050_SLEEP));
}

bool MPU6050::powerOff() {
	return setByteRegBits(MPU6050_PWR_MGMT_1, 1 << MPU6050_SLEEP);
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
