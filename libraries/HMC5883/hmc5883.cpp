#include <string.h>
#include <hmc5883.h>

struct HMC5883DataRegs {
	int16_t x;
	int16_t z;
	int16_t y;
} __attribute__((packed));

HMC5883::HMC5883(I2CBus& i2c, uint8_t addr) : SimpleI2CDevice(i2c, addr) {
	// Do nothing
}
		
bool HMC5883::detect() {
	char idString[3];
	if (!readReg(HMC5883_ID_A, idString, sizeof(idString))) return false;
	return strncmp(idString, "H43", 3) == 0;
}

bool HMC5883::setMeasurementMode(HMC5883MeasurementMode mode) {
	uint8_t value;
	if (!readReg(HMC5883_CONFIG_A, &value, sizeof(value))) return false;
	value = (value & ~(HMC5883_MS0 | HMC5883_MS1)) | mode;
	return writeByteReg(HMC5883_CONFIG_A, value);
}

bool HMC5883::setDataRate(HMC5883DataRate rate) {
	uint8_t value;
	if (!readReg(HMC5883_CONFIG_A, &value, sizeof(value))) return false;
	value = (value & ~(HMC5883_DO2 | HMC5883_DO1 | HMC5883_DO0)) | rate;
	return writeByteReg(HMC5883_CONFIG_A, value);
}

bool HMC5883::setGain(HMC5883Gain gain) {
	return writeByteReg(HMC5883_CONFIG_B, gain);
}

bool HMC5883::setMode(HMC5883Mode mode) {
	return writeByteReg(HMC5883_MODE, mode);
}
	
bool HMC5883::updateData() {
	volatile HMC5883DataRegs data;
	if (!readReg(HMC5883_DATA_XH, (void*)&data, sizeof(data))) return false;
	uint16_t *rawData = (uint16_t*)&data;
	for (unsigned int i = 0; i < (sizeof(data) / sizeof(uint16_t)); i++) {
		uint16_t value = rawData[i];
		rawData[i] = ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
	}
	_rawValueX = data.x;
	_rawValueY = data.y;
	_rawValueZ = data.z;
	return true;
}
		
