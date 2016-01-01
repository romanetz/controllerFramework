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

BOOL mpu6050_readData(MPU6050Class *mpu6050) {
	uint8_t txBuf[] = {MPU6050_ACCEL_XOUT_H};
	if (i2cMasterWriteReadTimeout(mpu6050->i2c, mpu6050->address, txBuf, sizeof(txBuf), &mpu6050->rawData, sizeof(mpu6050->rawData), MPU6050_I2C_TIMEOUT)) {
		uint16_t *words = (uint16_t*)&mpu6050->rawData;
		int i;
		for (i = 0; i < (sizeof(mpu6050->rawData) / sizeof(uint16_t)); i++) {
			words[i] = NTOHS(words[i]);
		}
		return TRUE;
	}
	return FALSE;
}