#ifndef __HMC5883_H__
#define __HMC5883_H__

#include <i2c.h>
#include <hmc5883regs.h>

typedef struct PACKED HMC5883RawData {
	int16_t magX;
	int16_t magZ;
	int16_t magY;
} HMC5883RawData;

#ifdef USE_CLASSES

#define HMC5883_I2C_TIMEOUT 5000

typedef struct HMC5883Class HMC5883Class;

struct HMC5883Class {
	I2CClass *i2c;
	uint8_t address;
	HMC5883RawData rawData;
	float magX, magXOffset, magXScale;
	float magY, magYOffset, magYScale;
	float magZ, magZOffset, magZScale;
};

#define HMC5883_CLASS(obj) ((HMC5883Class*)(obj))

HMC5883Class *hmc5883_classInit(HMC5883Class *hmc5883, I2CClass *i2c, uint8_t address);

BOOL hmc5883_writeByteReg(HMC5883Class *hmc5883, uint8_t reg, uint8_t value);
BOOL hmc5883_readByteReg(HMC5883Class *hmc5883, uint8_t reg, uint8_t *value);
BOOL hmc5883_andByteReg(HMC5883Class *hmc5883, uint8_t reg, uint8_t value);
BOOL hmc5883_orByteReg(HMC5883Class *hmc5883, uint8_t reg, uint8_t value);

BOOL hmc5883_detect(HMC5883Class *hmc5883);

BOOL hmc5883_setMeasurementMode(HMC5883Class *hmc5883, HMC5883MeasurementMode mode);
BOOL hmc5883_setDataRate(HMC5883Class *hmc5883, HMC5883DataRate rate);
BOOL hmc5883_setGain(HMC5883Class *hmc5883, HMC5883Gain gain);
BOOL hmc5883_setMode(HMC5883Class *hmc5883, HMC5883Mode mode);

BOOL hmc5883_setup(HMC5883Class *hmc5883, HMC5883DataRate rate, HMC5883Gain gain, HMC5883Mode mode, HMC5883MeasurementMode measurementMode);

BOOL hmc5883_readData(HMC5883Class *hmc5883);
void hmc5883_parseData(HMC5883Class *hmc5883);

#endif

#endif
