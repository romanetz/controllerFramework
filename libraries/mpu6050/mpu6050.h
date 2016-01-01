#ifndef __MPU6050_H__
#define __MPU6050_H__

#include <i2c.h>
#include <mpu6050regs.h>

typedef struct PACKED MPU6050Data {
	int16_t accelX;
	int16_t accelY;
	int16_t accelZ;
	int16_t temperature;
	int16_t gyroX;
	int16_t gyroY;
	int16_t gyroZ;
} MPU6050Data;

#ifdef USE_CLASSES

#define MPU6050_I2C_TIMEOUT 1000

typedef struct MPU6050Class MPU6050Class;

struct MPU6050Class {
	I2CClass *i2c;
	uint8_t address;
	MPU6050Data rawData;
};

MPU6050Class *mpu6050_classInit(MPU6050Class *mpu6050, I2CClass *i2c, uint8_t address);

BOOL mpu6050_writeByteReg(MPU6050Class *mpu6050, uint8_t reg, uint8_t value);
BOOL mpu6050_readByteReg(MPU6050Class *mpu6050, uint8_t reg, uint8_t *value);
BOOL mpu6050_andByteReg(MPU6050Class *mpu6050, uint8_t reg, uint8_t value);
BOOL mpu6050_orByteReg(MPU6050Class *mpu6050, uint8_t reg, uint8_t value);

BOOL mpu6050_detect(MPU6050Class *mpu6050);

BOOL mpu6050_powerOn(MPU6050Class *mpu6050);
BOOL mpu6050_powerOff(MPU6050Class *mpu6050);

BOOL mpu6050_readData(MPU6050Class *mpu6050);

#endif

#endif
