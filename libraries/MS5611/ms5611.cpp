#include <platform.h>
#include <ms5611.h>

MS5611::MS5611(I2CBus& i2c, uint8_t addr) : _i2c(i2c), _addr(addr) {
	
}

bool MS5611::detect() {
	return readPROM(1 << 1) != 0;
}

bool MS5611::sendCommand(uint8_t command, void *response, unsigned int responseSize) {
	uint8_t txBuf[] = {command};
	return _i2c.masterTxRx(_addr, txBuf, sizeof(txBuf), response, responseSize);
}
	
uint32_t MS5611::readADC() {
	uint8_t rxBuf[3];
	if (!sendCommand(MS5611_CMD_ADC_READ, rxBuf, sizeof(rxBuf))) return 0;
	return rxBuf[2] | (rxBuf[1] << 8) | (rxBuf[0] << 16);
}

uint16_t MS5611::readPROM(uint8_t addr) {
	uint16_t value;
	if (!sendCommand(MS5611_CMD_PROM_READ | (addr << 1), &value, sizeof(value))) return 0;
	return ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
}

bool MS5611::loadPROM() {
	for (int i = 0; i < 8; i++) {
		_prom[i] = readPROM(i);
	}
	return true;
}

bool MS5611::reset() {
	if (!sendCommand(MS5611_CMD_RESET)) return false;
	usleep(3000);
	return loadPROM();
}
		
bool MS5611::updateData() {
	return true;
}
