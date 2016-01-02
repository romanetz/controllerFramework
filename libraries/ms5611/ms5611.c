#include <platform.h>
#include <ms5611.h>

#ifdef USE_CLASSES

MS5611Class *ms5611_classInit(MS5611Class *ms5611, I2CClass *i2c, uint8_t addr) {
	ms5611->i2c = i2c;
	ms5611->address = addr;
	return ms5611;
}

BOOL ms5611_sendCommand(MS5611Class *ms5611, uint8_t command, void *response, int responseSize) {
	uint8_t opcode = command & 0xF0;
	if ((opcode == MS5611_CMD_CONVERT_D1) || (opcode == MS5611_CMD_CONVERT_D2)) {
		ms5611->lastConvertCommand = opcode;
	}
	uint8_t txBuf[] = {command};
	return i2cMasterWriteReadTimeout(ms5611->i2c, ms5611->address, txBuf, sizeof(txBuf), response, responseSize, MS5611_I2C_TIMEOUT);
}

BOOL ms5611_reset(MS5611Class *ms5611, BOOL loadPROM) {
	if (!ms5611_sendCommand(ms5611, MS5611_CMD_RESET, 0, 0)) return FALSE;
	usleep(3000);
	return !loadPROM || ms5611_loadPROM(ms5611, TRUE);
}

BOOL ms5611_readPROMValue(MS5611Class *ms5611, uint8_t address, uint16_t *value) {
	uint8_t data[2];
	if (!ms5611_sendCommand(ms5611, MS5611_CMD_PROM_READ | address, data, sizeof(data))) return FALSE;
	*value = (data[0] << 8) | data[1];
	return TRUE;
}

BOOL ms5611_readADC(MS5611Class *ms5611, uint32_t *value) {
	uint8_t data[3];
	if (!ms5611_sendCommand(ms5611, MS5611_CMD_ADC_READ, data, sizeof(data))) return FALSE;
	*value = data[2] | (data[1] << 8) | (data[0] << 16);
	if (ms5611->lastConvertCommand == MS5611_CMD_CONVERT_D1) {
		ms5611->rawPressure = *value;
	} else if (ms5611->lastConvertCommand == MS5611_CMD_CONVERT_D2) {
		ms5611->rawTemperature = *value;
	}
	return TRUE;
}

BOOL ms5611_loadPROM(MS5611Class *ms5611, BOOL check) {
	int i;
	for (i = 0; i < 8; i++) {
		if (!ms5611_readPROMValue(ms5611, i * 2, ms5611->prom + i)) {
			return FALSE;
		}
	}
	return !check || ms5611_checkPROM(ms5611);
}

BOOL ms5611_checkPROM(MS5611Class *ms5611) {
	uint16_t savedCrc = ms5611->prom[7];
	ms5611->prom[7] &= ~0xFF;
	uint16_t r = 0;
	unsigned int i;
	for (i = 0; i < sizeof(ms5611->prom); i++) {
		uint8_t byte = ((i % 2) == 1) ? (ms5611->prom[i / 2] & 0xFF) : (ms5611->prom[i / 2] >> 8);
		r ^= byte;
		for (int j = 8; j > 0; j--) {
			if (r & 0x8000) {
				r = (r << 1) ^ 0x3000;
			} else {
				r = r << 1;
			}
		}
	}
	r = (r >> 12) & 0x000F;
	ms5611->prom[7] = savedCrc;
	return (savedCrc & 0x000F) == r;
}

BOOL ms5611_detect(MS5611Class *ms5611) {
	uint32_t value;
	return ms5611_readADC(ms5611, &value);
}

BOOL ms5611_convert(MS5611Class *ms5611, MS5611OSR osr) {
	uint8_t lastConvertCommand = ms5611->lastConvertCommand;
	uint32_t value;
	if (!ms5611_readADC(ms5611, &value)) return FALSE;
	if (lastConvertCommand == MS5611_CMD_CONVERT_D2) {
		if (!ms5611_sendCommand(ms5611, MS5611_CMD_CONVERT_D1 | osr, 0, 0)) return FALSE;
		ms5611->dT = ms5611->rawTemperature - (uint32_t)ms5611->prom[5] * 256;
		ms5611->temp = 2000 + ((int64_t)ms5611->dT * ms5611->prom[6]) / 8388608;
		if (ms5611->temp < 2000) {
			ms5611->temp -= (int64_t)(ms5611->dT * ms5611->dT) / (2 << 30);
		}
		ms5611->temperature = ms5611->temp / 100.0f;
	} else {
		if (!ms5611_sendCommand(ms5611, MS5611_CMD_CONVERT_D2 | osr, 0, 0)) return FALSE;
		int64_t off = (int64_t)(ms5611->prom[2]) * 65536 + ((int64_t)(ms5611->prom[4]) * ms5611->dT) / 128;
		int64_t sens = (int64_t)(ms5611->prom[1]) * 32768 + ((int64_t)(ms5611->prom[3]) * ms5611->dT) / 256;
		if (ms5611->temp < 2000) {
			uint32_t t = ms5611->temp - 2000;
			off -= 5 * t * t / 2;
			sens -= 5 * t * t / 4;
			if (ms5611->temp < -1500) {
				t = ms5611->temp + 1500;
				off -= 7 * t * t;
				sens -= 11 * t * t / 2;
			}
		}
		ms5611->pressure = ((ms5611->rawPressure * sens / 2097152) - off) / 32768;
	}
	return TRUE;
}

#endif