#include <platform.h>
#include <ms5611.h>

MS5611::MS5611(I2CBus& i2c, uint8_t addr, MS5611OSR osr) : _i2c(i2c), _addr(addr) {
	setOSR(osr);
}

bool MS5611::detect() {
	return readPROM(1 << 1) != 0;
}

bool MS5611::setup() {
	if (!reset()) return false;
	if (!sendCommand(MS5611_CMD_CONVERT_D2)) return false;
	return true;
}

bool MS5611::sendCommand(uint8_t command, void *response, unsigned int responseSize) {
	uint8_t opcode = command & 0xF0;
	if ((opcode == MS5611_CMD_CONVERT_D1) || (opcode == MS5611_CMD_CONVERT_D2)) {
		_lastConvertCommand = opcode;
	}
	uint8_t txBuf[] = {command};
	return _i2c.masterTxRx(_addr, txBuf, sizeof(txBuf), response, responseSize);
}
	
uint32_t MS5611::readADC() {
	uint32_t value = 0;
	uint8_t rxBuf[3];
	if (sendCommand(MS5611_CMD_ADC_READ, rxBuf, sizeof(rxBuf))) {
		value = rxBuf[2] | (rxBuf[1] << 8) | (rxBuf[0] << 16);
		if (_lastConvertCommand == MS5611_CMD_CONVERT_D1) {
			_rawPressure = value;
		} else if (_lastConvertCommand == MS5611_CMD_CONVERT_D2) {
			_rawTemperature = value;
		}
	}
	_lastConvertCommand = 0;
	return value;
}

uint16_t MS5611::readPROM(uint8_t addr) {
	uint16_t value;
	if (!sendCommand(MS5611_CMD_PROM_READ | (addr << 1), &value, sizeof(value))) return 0;
	return ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
}

bool MS5611::loadPROM(bool check) {
	for (int i = 0; i < 8; i++) {
		_prom[i] = readPROM(i);
	}
	return !check || checkPROM();
}

bool MS5611::checkPROM() {
	uint16_t savedCrc = _prom[7];
	_prom[7] &= ~0xFF;
	uint16_t r = 0;
	for (unsigned int i = 0; i < sizeof(_prom); i++) {
		uint8_t byte = ((i % 2) == 1) ? (_prom[i / 2] & 0xFF) : (_prom[i / 2] >> 8);
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
	_prom[7] = savedCrc;
	return (savedCrc & 0x000F) == r;
}

bool MS5611::reset(bool reloadPROM) {
	if (!sendCommand(MS5611_CMD_RESET)) return false;
	usleep(3000);
	return !reloadPROM || loadPROM(true);
}
		
bool MS5611::updateData(bool convert) {
	uint8_t lastConvertCommand = _lastConvertCommand;
	if (readADC() == 0) return false;
	if (lastConvertCommand == MS5611_CMD_CONVERT_D2) {
		sendCommand(MS5611_CMD_CONVERT_D1 | _osr);
		if (convert) {
			_dT = _rawTemperature - (uint32_t)_prom[5] * 256;
			_temperature = 2000 + ((int64_t)_dT * _prom[6]) / 8388608;
			if (_temperature < 2000) {
				_temperature -= (int64_t)(_dT * _dT) / (2 << 30);
			}
		}
	} else {
		sendCommand(MS5611_CMD_CONVERT_D2 | _osr);
		if (convert) {
			int64_t off = (int64_t)(_prom[2]) * 65536 + ((int64_t)(_prom[4]) * _dT) / 128;
			int64_t sens = (int64_t)(_prom[1]) * 32768 + ((int64_t)(_prom[3]) * _dT) / 256;
			if (_temperature < 2000) {
				uint32_t t = _temperature - 2000;
				off -= 5 * t * t / 2;
				sens -= 5 * t * t / 4;
				if (_temperature < -1500) {
					t = _temperature + 1500;
					off -= 7 * t * t;
					sens -= 11 * t * t / 2;
				}
			}
			_pressure = ((_rawPressure * sens / 2097152) - off) / 32768;
		}
	}
	return true;
}
