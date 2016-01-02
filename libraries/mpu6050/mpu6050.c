#include <mpu6050.h>

MPU6050Class *mpu6050_classInit(MPU6050Class *mpu6050, I2CClass *i2c, uint8_t address) {
	mpu6050->i2c = i2c;
	mpu6050->address = address;
	return mpu6050;
}

BOOL mpu6050_writeByteReg(MPU6050Class *mpu6050, uint8_t reg, uint8_t value) {
	uint8_t txBuf[] = {reg, value};
	return i2cMasterWriteReadTimeout(mpu6050->i2c, mpu6050->address, txBuf, sizeof(txBuf), 0, 0, MPU6050_I2C_TIMEOUT);
}

BOOL mpu6050_readByteReg(MPU6050Class *mpu6050, uint8_t reg, uint8_t *value) {
	uint8_t txBuf[] = {reg};
	uint8_t rxBuf[1];
	if (i2cMasterWriteReadTimeout(mpu6050->i2c, mpu6050->address, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), MPU6050_I2C_TIMEOUT)) {
		*value = rxBuf[0];
		return TRUE;
	}
	return FALSE;
}

BOOL mpu6050_andByteReg(MPU6050Class *mpu6050, uint8_t reg, uint8_t value) {
	uint8_t regValue;
	if (mpu6050_readByteReg(mpu6050, reg, &regValue)) {
		regValue &= value;
		if (mpu6050_writeByteReg(mpu6050, reg, regValue)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL mpu6050_orByteReg(MPU6050Class *mpu6050, uint8_t reg, uint8_t value) {
	uint8_t regValue;
	if (mpu6050_readByteReg(mpu6050, reg, &regValue)) {
		regValue |= value;
		if (mpu6050_writeByteReg(mpu6050, reg, regValue)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL mpu6050_detect(MPU6050Class *mpu6050) {
	uint8_t value = 0x00;
	mpu6050_readByteReg(mpu6050, MPU6050_WHO_AM_I, &value);
	return value == 0x68;
}

BOOL mpu6050_powerOn(MPU6050Class *mpu6050) {
	return mpu6050_andByteReg(mpu6050, MPU6050_PWR_MGMT_1, (uint8_t)~((1 << MPU6050_DEVICE_RESET) | (1 << MPU6050_SLEEP)));
}

BOOL mpu6050_powerOff(MPU6050Class *mpu6050) {
	return mpu6050_orByteReg(mpu6050, MPU6050_PWR_MGMT_1, 1 << MPU6050_SLEEP);
}

BOOL mpu6050_setAccelScale(MPU6050Class *mpu6050, MPU6050AccelScale scale) {
	if (!mpu6050_writeByteReg(mpu6050, MPU6050_ACCEL_CONFIG, scale)) return FALSE;
	float scaleValue = 1.0f;
	switch (scale) {
		case MPU6050_ACCELSCALE_2G:
			scaleValue = 1.0f / 16384.0f;
			break;
		case MPU6050_ACCELSCALE_4G:
			scaleValue = 1.0f / 8192.0f;
			break;
		case MPU6050_ACCELSCALE_8G:
			scaleValue = 1.0f / 4096.0f;
			break;
		case MPU6050_ACCELSCALE_16G:
			scaleValue = 1.0f / 2048.0f;
			break;
	}
	mpu6050->accelXOffset = 0.0f;
	mpu6050->accelXScale = scaleValue;
	mpu6050->accelYOffset = 0.0f;
	mpu6050->accelYScale = scaleValue;
	mpu6050->accelZOffset = 0.0f;
	mpu6050->accelZScale = scaleValue;
	return TRUE;
}

BOOL mpu6050_setGyroScale(MPU6050Class *mpu6050, MPU6050GyroScale scale) {
	if (!mpu6050_writeByteReg(mpu6050, MPU6050_GYRO_CONFIG, scale)) return FALSE;
	float scaleValue = 1.0f;
	switch (scale) {
		case MPU6050_GYROSCALE_250DEG:
			scaleValue = 1.0f / 131.0f;
			break;
		case MPU6050_GYROSCALE_500DEG:
			scaleValue = 1.0f / 65.5f;
			break;
		case MPU6050_GYROSCALE_1000DEG:
			scaleValue = 1.0f / 32.8f;
			break;	
		case MPU6050_GYROSCALE_2000DEG:
			scaleValue = 1.0f / 16.4f;
			break;
	}
	mpu6050->gyroXOffset = 0.0f;
	mpu6050->gyroXScale = scaleValue;
	mpu6050->gyroYOffset = 0.0f;
	mpu6050->gyroYScale = scaleValue;
	mpu6050->gyroZOffset = 0.0f;
	mpu6050->gyroZScale = scaleValue;
	return TRUE;
}

BOOL mpu6050_enableI2CByPass(MPU6050Class *mpu6050) {
	return mpu6050_orByteReg(mpu6050, MPU6050_INT_PIN_CFG, 1 << MPU6050_I2C_BYPASS_EN);
}

BOOL mpu6050_disableI2CByPass(MPU6050Class *mpu6050) {
	return mpu6050_andByteReg(mpu6050, MPU6050_INT_PIN_CFG, ~(1 << MPU6050_I2C_BYPASS_EN));
}

BOOL mpu6050_setup(MPU6050Class *mpu6050, MPU6050AccelScale accelScale, MPU6050GyroScale gyroScale, BOOL i2cByPass) {
	if (!mpu6050_powerOn(mpu6050)) return FALSE;
	if (!mpu6050_setAccelScale(mpu6050, accelScale)) return FALSE;
	if (!mpu6050_setGyroScale(mpu6050, gyroScale)) return FALSE;
	if (i2cByPass) {
		if (!mpu6050_enableI2CByPass(mpu6050)) return FALSE;
	} else {
		if (!mpu6050_disableI2CByPass(mpu6050)) return FALSE;
	}
	return TRUE;
}

BOOL mpu6050_readData(MPU6050Class *mpu6050) {
	uint8_t txBuf[] = {MPU6050_ACCEL_XOUT_H};
	if (i2cMasterWriteReadTimeout(mpu6050->i2c, mpu6050->address, txBuf, sizeof(txBuf), &mpu6050->rawData, sizeof(mpu6050->rawData), MPU6050_I2C_TIMEOUT)) {
		uint8_t *bytes = (uint8_t*)&mpu6050->rawData;
		uint16_t *words = (uint16_t*)&mpu6050->rawData;
		int i;
		for (i = 0; i < (sizeof(mpu6050->rawData) / sizeof(uint16_t)); i++) {
			uint8_t h = bytes[i * 2 + 0];
			uint8_t l = bytes[i * 2 + 1];
			words[i] = l | (h << 8);
		}
		return TRUE;
	}
	return FALSE;
}

void mpu6050_parseData(MPU6050Class *mpu6050) {
	mpu6050->accelX = mpu6050->accelXScale * mpu6050->rawData.accelX + mpu6050->accelXOffset;
	mpu6050->accelY = mpu6050->accelYScale * mpu6050->rawData.accelY + mpu6050->accelYOffset;
	mpu6050->accelZ = mpu6050->accelZScale * mpu6050->rawData.accelZ + mpu6050->accelZOffset;
	mpu6050->gyroX = mpu6050->gyroXScale * mpu6050->rawData.gyroX + mpu6050->gyroXOffset;
	mpu6050->gyroY = mpu6050->gyroYScale * mpu6050->rawData.gyroY + mpu6050->gyroYOffset;
	mpu6050->gyroZ = mpu6050->gyroZScale * mpu6050->rawData.gyroZ + mpu6050->gyroZOffset;
	mpu6050->temperature = mpu6050->rawData.temperature / 340.f + 36.53f;
}