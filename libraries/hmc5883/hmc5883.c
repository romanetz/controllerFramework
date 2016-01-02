#include <string.h>
#include <hmc5883.h>

HMC5883Class *hmc5883_classInit(HMC5883Class *hmc5883, I2CClass *i2c, uint8_t address) {
	hmc5883->i2c = i2c;
	hmc5883->address = address;
	return hmc5883;
}

BOOL hmc5883_writeByteReg(HMC5883Class *hmc5883, uint8_t reg, uint8_t value) {
	uint8_t txBuf[] = {reg, value};
	return i2cMasterWriteReadTimeout(hmc5883->i2c, hmc5883->address, txBuf, sizeof(txBuf), 0, 0, HMC5883_I2C_TIMEOUT);
}

BOOL hmc5883_readByteReg(HMC5883Class *hmc5883, uint8_t reg, uint8_t *value) {
	uint8_t txBuf[] = {reg};
	uint8_t rxBuf[1];
	if (i2cMasterWriteReadTimeout(hmc5883->i2c, hmc5883->address, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), HMC5883_I2C_TIMEOUT)) {
		*value = rxBuf[0];
		return TRUE;
	}
	return FALSE;
}

BOOL hmc5883_andByteReg(HMC5883Class *hmc5883, uint8_t reg, uint8_t value) {
	uint8_t regValue;
	if (hmc5883_readByteReg(hmc5883, reg, &regValue)) {
		regValue &= value;
		if (hmc5883_writeByteReg(hmc5883, reg, regValue)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL hmc5883_orByteReg(HMC5883Class *hmc5883, uint8_t reg, uint8_t value) {
	uint8_t regValue;
	if (hmc5883_readByteReg(hmc5883, reg, &regValue)) {
		regValue |= value;
		if (hmc5883_writeByteReg(hmc5883, reg, regValue)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL hmc5883_detect(HMC5883Class *hmc5883) {
	uint8_t txBuf[] = {HMC5883_ID_A};
	char idString[3];
	i2cMasterWriteReadTimeout(hmc5883->i2c, hmc5883->address, txBuf, sizeof(txBuf), idString, sizeof(idString), HMC5883_I2C_TIMEOUT);
	return strncmp(idString, "H43", 3) == 0;
}

BOOL hmc5883_setMeasurementMode(HMC5883Class *hmc5883, HMC5883MeasurementMode mode) {
	uint8_t value;
	if (!hmc5883_readByteReg(hmc5883, HMC5883_CONFIG_A, &value)) return FALSE;
	value = (value & ~(HMC5883_MS0 | HMC5883_MS1)) | mode;
	return hmc5883_writeByteReg(hmc5883, HMC5883_CONFIG_A, value);
}

BOOL hmc5883_setDataRate(HMC5883Class *hmc5883, HMC5883DataRate rate) {
	uint8_t value;
	if (!hmc5883_readByteReg(hmc5883, HMC5883_CONFIG_A, &value)) return FALSE;
	value = (value & ~(HMC5883_DO2 | HMC5883_DO1 | HMC5883_DO0)) | rate;
	return hmc5883_writeByteReg(hmc5883, HMC5883_CONFIG_A, value);
}

BOOL hmc5883_setGain(HMC5883Class *hmc5883, HMC5883Gain gain) {
	if (!hmc5883_writeByteReg(hmc5883, HMC5883_CONFIG_B, gain)) return FALSE;
	float scale = 1.0f;
	switch (gain) {
		case HMC5883_GAIN_0_9GA:
			scale = 1.0f / 1280.0f;
			break;
		case HMC5883_GAIN_1_2GA:
			scale = 1.0f / 1024.0f;
			break;
		case HMC5883_GAIN_1_9GA:
			scale = 1.0f / 768.0f;
			break;
		case HMC5883_GAIN_2_5GA:
			scale = 1.0f / 614.0f;
			break;
		case HMC5883_GAIN_4GA:
			scale = 1.0f / 415.0f;
			break;
		case HMC5883_GAIN_4_6GA:
			scale = 1.0f / 361.0f;
			break;
		case HMC5883_GAIN_5_5GA:
			scale = 1.0f / 307.0f;
			break;
		case HMC5883_GAIN_7_9GA:
			scale = 1.0f / 219.0f;
			break;
	}
	hmc5883->magXOffset = 0.0f;
	hmc5883->magXScale = scale;
	hmc5883->magYOffset = 0.0f;
	hmc5883->magYScale = scale;
	hmc5883->magZOffset = 0.0f;
	hmc5883->magZScale = scale;
	return TRUE;
}

BOOL hmc5883_setMode(HMC5883Class *hmc5883, HMC5883Mode mode) {
	return hmc5883_writeByteReg(hmc5883, HMC5883_MODE, mode);
}

BOOL hmc5883_setup(HMC5883Class *hmc5883, HMC5883DataRate rate, HMC5883Gain gain, HMC5883Mode mode, HMC5883MeasurementMode measurementMode) {
	if (!hmc5883_setDataRate(hmc5883, rate)) return FALSE;
	if (!hmc5883_setGain(hmc5883, gain)) return FALSE;
	if (!hmc5883_setMeasurementMode(hmc5883, measurementMode)) return FALSE;
	if (!hmc5883_setMode(hmc5883, mode)) return FALSE;
	return TRUE;
}

BOOL hmc5883_readData(HMC5883Class *hmc5883) {
	uint8_t txBuf[] = {HMC5883_DATA_XH};
	if (i2cMasterWriteReadTimeout(hmc5883->i2c, hmc5883->address, txBuf, sizeof(txBuf), &hmc5883->rawData, sizeof(hmc5883->rawData), HMC5883_I2C_TIMEOUT)) {
		uint8_t *bytes = (uint8_t*)&hmc5883->rawData;
		uint16_t *words = (uint16_t*)&hmc5883->rawData;
		int i;
		for (i = 0; i < (sizeof(hmc5883->rawData) / sizeof(uint16_t)); i++) {
			uint8_t h = bytes[i * 2 + 0];
			uint8_t l = bytes[i * 2 + 1];
			words[i] = l | (h << 8);
		}
		return TRUE;
	}
	return FALSE;
}

void hmc5883_parseData(HMC5883Class *hmc5883) {
	hmc5883->magX = hmc5883->magXScale * hmc5883->rawData.magX + hmc5883->magXOffset;
	hmc5883->magY = hmc5883->magYScale * hmc5883->rawData.magY + hmc5883->magYOffset;
	hmc5883->magZ = hmc5883->magZScale * hmc5883->rawData.magZ + hmc5883->magZOffset;
}