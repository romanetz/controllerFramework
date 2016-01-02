#ifndef __MPU6050_H__
#define __MPU6050_H__

#include <i2c.h>
#include <mpu6050regs.h>

typedef struct PACKED MPU6050RawData {
	int16_t accelX;
	int16_t accelY;
	int16_t accelZ;
	int16_t temperature;
	int16_t gyroX;
	int16_t gyroY;
	int16_t gyroZ;
} MPU6050RawData;

#ifdef USE_CLASSES

#define MPU6050_I2C_TIMEOUT 5000

typedef struct MPU6050Class MPU6050Class;

typedef enum MPU6050GyroScale {
	MPU6050_GYROSCALE_250DEG = MPU6050_FS_SEL_250,
	MPU6050_GYROSCALE_500DEG = MPU6050_FS_SEL_500,
	MPU6050_GYROSCALE_1000DEG = MPU6050_FS_SEL_1000,
	MPU6050_GYROSCALE_2000DEG = MPU6050_FS_SEL_2000
} MPU6050GyroScale;

typedef enum MPU6050AccelScale {
	MPU6050_ACCELSCALE_2G = MPU6050_AFS_SEL_2G,
	MPU6050_ACCELSCALE_4G = MPU6050_AFS_SEL_4G,
	MPU6050_ACCELSCALE_8G = MPU6050_AFS_SEL_8G,
	MPU6050_ACCELSCALE_16G = MPU6050_AFS_SEL_16G
} MPU6050AccelScale;

struct MPU6050Class {
	I2CClass *i2c;
	uint8_t address;
	MPU6050RawData rawData;
	float accelX, accelXOffset, accelXScale;
	float accelY, accelYOffset, accelYScale;
	float accelZ, accelZOffset, accelZScale;
	float gyroX, gyroXOffset, gyroXScale;
	float gyroY, gyroYOffset, gyroYScale;
	float gyroZ, gyroZOffset, gyroZScale;
	float temperature;
};

MPU6050Class *mpu6050_classInit(MPU6050Class *mpu6050, I2CClass *i2c, uint8_t address);

BOOL mpu6050_writeByteReg(MPU6050Class *mpu6050, uint8_t reg, uint8_t value);
BOOL mpu6050_readByteReg(MPU6050Class *mpu6050, uint8_t reg, uint8_t *value);
BOOL mpu6050_andByteReg(MPU6050Class *mpu6050, uint8_t reg, uint8_t value);
BOOL mpu6050_orByteReg(MPU6050Class *mpu6050, uint8_t reg, uint8_t value);

BOOL mpu6050_detect(MPU6050Class *mpu6050);

BOOL mpu6050_powerOn(MPU6050Class *mpu6050);
BOOL mpu6050_powerOff(MPU6050Class *mpu6050);
BOOL mpu6050_setAccelScale(MPU6050Class *mpu6050, MPU6050AccelScale scale);
BOOL mpu6050_setGyroScale(MPU6050Class *mpu6050, MPU6050GyroScale scale);
BOOL mpu6050_enableI2CByPass(MPU6050Class *mpu6050);
BOOL mpu6050_disableI2CByPass(MPU6050Class *mpu6050);

BOOL mpu6050_setup(MPU6050Class *mpu6050, MPU6050AccelScale accelScale, MPU6050GyroScale gyroScale, BOOL i2cByPass);

BOOL mpu6050_readData(MPU6050Class *mpu6050);
void mpu6050_parseData(MPU6050Class *mpu6050);

#endif

#endif
