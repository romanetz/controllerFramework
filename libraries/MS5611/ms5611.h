#ifndef __MS5611_H__
#define __MS5611_H__

#include <i2c-bus.h>
#include <ms5611regs.h>

class MS5611 {
	private:
		I2CBus& _i2c;
		
		uint8_t _addr;
		
		int16_t _temperature;
		
		uint32_t _pressure;
		
		uint16_t _prom[8];
	
	public:
		MS5611(I2CBus& i2c, uint8_t addr);
		
		bool detect();
		
		bool sendCommand(uint8_t command, void *response = 0, unsigned int responseSize = 0);
		
		uint32_t readADC();
		
		uint16_t readPROM(uint8_t addr);
		
		bool loadPROM();
		
		bool reset();
		
		bool updateData();
		
		int16_t temperature() { return _temperature; }
		
		uint32_t pressure() { return _pressure; }
		
		uint16_t prom(uint8_t addr) { return _prom[addr]; }
		
};

#endif
