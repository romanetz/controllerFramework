#ifndef __MS5611_H__
#define __MS5611_H__

#include <i2c.h>
#include <ms5611regs.h>

#ifdef USE_CLASSES

typedef struct MS5611Class MS5611Class;

#define MS5611_I2C_TIMEOUT 5000

struct MS5611Class {
	I2CClass *i2c;
	uint8_t address;
	uint8_t lastConvertCommand;
	uint16_t prom[8];
	uint32_t rawTemperature;
	uint32_t rawPressure;
	int32_t dT;
	int32_t temp;
	uint32_t pressure;
	float temperature;
};

#define MS5611_CLASS(obj) ((MS5611Class*)(obj))

MS5611Class *ms5611_classInit(MS5611Class *ms5611, I2CClass *i2c, uint8_t addr);

BOOL ms5611_sendCommand(MS5611Class *ms5611, uint8_t command, void *response, int responseSize);
BOOL ms5611_reset(MS5611Class *ms5611, BOOL loadPROM);
BOOL ms5611_readPROMValue(MS5611Class *ms5611, uint8_t address, uint16_t *value);
BOOL ms5611_readADC(MS5611Class *ms5611, uint32_t *value);
BOOL ms5611_loadPROM(MS5611Class *ms5611, BOOL check);
BOOL ms5611_checkPROM(MS5611Class *ms5611);

BOOL ms5611_detect(MS5611Class *ms5611);
BOOL ms5611_convert(MS5611Class *ms5611, MS5611OSR osr);

#endif

#endif
